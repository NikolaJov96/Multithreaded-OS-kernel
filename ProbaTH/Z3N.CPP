//pretpostavljeni memorijski model: huge

#include <iostream.h>
#include <dos.h>

// Zabranjuje prekide
#define lock asm cli

// Dozvoljava prekide
#define unlock asm sti

// Process Control Block
// po jedan za svaku nit
struct PCB
{
	unsigned sp;		// Stack pointer segment
	unsigned ss;		// Stack pointer offset
	unsigned zavrsio;	// Indikator da je nit zavrsila izvrsavanje
	int kvant;			// Vreme koje trenutna nit ima na raspolaganju u cugu
};

PCB *p[3];				// U ovom primeru imamo 3 niti (3 PCB-a), jedna je main
volatile PCB* running;	// Trenutno aktivna nit (PCB)

volatile int nextThread = 2;	// Naredna nit za izvrsavanje
PCB* getNextPCBToExecute() // Postavljanje naredne niti
{ 
	if (nextThread == 1) // zameni 1 - 2
		nextThread = 2;
	else nextThread = 1;
	
	if (p[nextThread]->zavrsio) // ako je novi vec zavrsen, vrati prethodni
	{
		if (nextThread == 1)
			nextThread = 2;
		else nextThread = 1;
		
		if (p[nextThread]->zavrsio)	// ako su oba zavrsena, vrati na main (0)
			nextThread = 0;
	}
	return p[nextThread];
}


// pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;

// volatile - uvek pristupaj promenljivama u RAM-u, ne optimizuj
volatile int brojac = 20;						// Brojac do istika vremena trenutnoj niti
volatile int zahtevana_promena_konteksta = 0;	// Indikator da je zahtevana promena konteksta
volatile unsigned int lockFlag = 1;				// Flag koji zabranjuje dispatch()

// prekidna rutina koju inicializuje procesor 
// na fiksan period vremena
void interrupt timer()
{	
	if (!zahtevana_promena_konteksta) // ako nije zahtevana promena konteksta brojac preostalig vremna se smanjuje
		brojac--; 
	
	if (brojac == 0 || zahtevana_promena_konteksta) // ako je zahtevana promena konteksta ili je brojac pao na nulu, promena konteksta
	{
		if (lockFlag) 	// ako nije postavljen lockFlag, mora da se saceka da se on postavi da bi se promenio kontekst
						// lockFlag se uklanja kada se izvrsavaju operacije 
		{
			zahtevana_promena_konteksta = 0; // kontekst se menja, tako da se eventualni zahtev ponistava (prihvata)
			
			// cuva trenutni sp u pomocne promenljive
			asm {
				mov tsp, sp
				mov tss, ss
			}

			running->sp = tsp; // Cuva trenutni sp preko pomocnih promenljivih u svoj PCB (na koji pokazuje running)
			running->ss = tss;

			lockFlag = 0; // Zabrana promene konteksta za vreme IO operacija
			cout << "Promena konteksta! Brojac= " << brojac << " running sp: " << running->sp << " " << running->ss << endl;
			asm cli;
			lockFlag = 1;
			
			running = getNextPCBToExecute(); // Scheduler - odabira PCB niti koja ce nastaviti izvrsavanje i running preusmerava na njega
	  
			tsp = running->sp; // Cuvanje novog sp u pomocne promenljive
			tss = running->ss; 

			brojac = running->kvant; // Novoj niti se daje onoliko vremena koliko je predvidjeno za nju

			asm {
				mov sp, tsp  // restaturacija sp procesora sp-om nove niti, pomocu pomocnih promenljivih
				mov ss, tss
			}
		} 
		else // ako je promena konteksta prekinuta zbog lockFlag-a, sledeci put obavezno menjamo kontekst
			zahtevana_promena_konteksta = 1; // trebalo bi da je ovo uvek tako i bez eksplicitnog postavljanja
		     
	} 
    
	// poziv stare prekidne rutine koja se 
    // nalazila na 08h, a sad je na 60h
    // poziva se samo kada nije zahtevana promena
    // konteksta – tako se da se stara
    // rutina poziva samo kada je stvarno doslo do prekida	
	if(!zahtevana_promena_konteksta) asm int 60h;
}

void dispatch() // sinhrona promena konteksta - kada se negde kesplicitno trazi promena (umesto prilikom prekida)
{ 
	asm cli; // onemogucavanje maskirajucih prekida tokom promene konteksta
	zahtevana_promena_konteksta = 1; // zahtev se postavlja na 1 i simulira se prekid
	timer();
	asm sti;
}

unsigned oldTimerOFF, oldTimerSEG; // segment i offset stare prekidne rutine

// zove se jednom na pocetku main-a, postavlja timer() kao difoltnu prekidnu rutinu
void inic(){
	asm{
		cli // onemogucavanje maskirajucih prekida
		push es // cuvanje registara koji ce biti korisceni
		push ax

		mov ax,0   // inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax	
		mov ax, word ptr es:0020h	
		mov word ptr oldTimerOFF, ax	

		mov word ptr es:0022h, seg timer	//postavlja 
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu na int 60h
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

// zove se jednom na kraju izvrsavanja programa, vraca stari prekidnu rutinu
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}

// zove se na kraju funkcija nad kojima se kreiraju niti
void exitThread(){
	running->zavrsio = 1; // flega zavrsetka niti se postavlja na 1
	dispatch();	// eksplicitno se zahteva promena konteksta
}  

// prvi primer funkcije nad kojom se pravi nit
void a(){
	for (int i =0; i < 30; ++i) 
	{
		lockFlag = 0; // onemogucavanje promene konteksta tokom IO operacija
		cout << "u a() i = " << i << endl;
		lockFlag = 1;
		if (zahtevana_promena_konteksta) // provera da li je tokom zabrane promene konteksta doslo pokusaja istog
			dispatch(); // ako jeste sada se prihvata
		for (int k = 0; k<10000; ++k) // cekanje malo vremena da bi se lepo videli rezultati primera
			for (int j = 0; j <30000; ++j);
	}
	exitThread(); // kraj niti, signal da je nit zavrsila
}

// drugi primer funkcije nad kojom se pravi nit
void b(){
	for (int i =0; i < 30; ++i) 
	{
		lockFlag = 0; // onemogucavanje promene konteksta tokom IO operacija
		cout << "u b() i = " << i << endl;
		lockFlag = 1;
		if (zahtevana_promena_konteksta) // provera da li je tokom zabrane promene konteksta doslo pokusaja istog
			dispatch(); // ako jeste sada se prihvata
		for (int k = 0; k<10000; ++k) // cekanje malo vremena da bi se lepo videli rezultati primera
			for (int j = 0; j <30000; ++j);
	}
	exitThread(); // kraj niti, signal da je nit zavrsila
}

// pravi proces (nit) i nad pridruzenim PCB-om i prilozenom funkcijom koja treba da se izvrsava
void createProcess(PCB *newPCB, void (*body)()){
	unsigned* st1 = new unsigned[1024]; // alokacija steka za novu nit - duzina 1024 unsigned int-a

	st1[1023] =0x200; // poctni PSW za novu nit - setovan I fleg - na vrhu steka
	st1[1022] = FP_SEG(body); // pokazivac na funkciju nad kojom je nit kreirana
	st1[1021] = FP_OFF(body);

	newPCB->sp = FP_OFF(st1+1012); // postavljanje inicijalnih vrednosti PCB-a nove niti
	newPCB->ss = FP_SEG(st1+1012);
	newPCB->zavrsio = 0;
}

// funkcija koja inicijalizuje niti i ceka neko vreme dok se one izvrse
void doSomething(){
	lock // <=> asm cli - zabrana prekida dok se inicijalizuju niti
	p[1] = new PCB();
	createProcess(p[1],a);
	cout<<"napravio a"<<endl;
	p[1]->kvant = 40;

	p[2] = new PCB();
	createProcess(p[2],b);
	cout<<"napravio b"<<endl;
	p[2]->kvant = 20;

	p[0] = new PCB(); // pravi se i postavlja neinicijalizovani PCB, 
					  //koji ce se pravilno inicijalizovati prilikom prvog poziva timer()

	running = p[0];
	unlock // <=> asm sti 

	for (int i = 0; i < 30; ++i) {
		lock
		cout<<"main "<<i<<endl;
		unlock

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
	cout<<"Happy End"<<endl;
}

int main(){
	
  inic();

  doSomething();
  
  restore();
 
  return 0;
}

