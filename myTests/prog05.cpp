int global = 0;
int signaled1 = 0, signaled2 = 0, sent1 = 0, sent2 = 0, paused1 = 0, paused2 = 0;
 
void tick(){global++;}
 
class TestThread : public Thread
{
public:
 
    TestThread(int idx, TestThread* stop = 0): Thread(1024, 5), i(idx), stop_(stop) {};
    ~TestThread()
    {
        waitToComplete();
    }
protected:
 
    void run();
private:
    int i;
    TestThread *stop_;
};
 
TestThread *t0, *t1, *t2;
 
 
void TestThread::run()
{
    while(1)
    {
        if (global%20 == 0)
        {
            //lockDis(cout<<i<<endl;)
        }
 
        //lockDis(cout<<"Global ="<<global<<endl;)
        if (i == 1)
        {
            if (global >= 50 && !paused1)
            {
                Thread::pause();
                paused1 = 1;
            }
 
            if (global >= 100 && stop_ != 0 && !signaled1)
            {
                stop_->signal(0);
                signaled1 = 1;
            }
 
            if (global >=200 && !sent2)
            {
                cout<<"SIGNALIZIRAO";
                t2->signal(2);
                sent2 = 1;
            }
        }
        if (i == 2)
        {
            if (global >= 100 && !sent1)
            {
                t1->signal(1);
                sent1 = 1;
            }
 
            if (global >= 150 && !paused2)
            {
                Thread::pause();
                paused2 = 1;
            }
 
            if (global >= 200 && stop_ != 0 && !signaled2)
            {
                stop_->signal(0);
                signaled2 = 1;
            }
            if (global >= 300)
                break;
        }
    }
}
 
 
void handler1()
{
    cout<<"PRIMIO1!!!!!!!!!!!"<<endl<<endl<<endl;
            for (int i=0; i<50; ++i)
                cout<<endl;
}
 
void handler2()
{
    cout<<"PRIMIO2!!!!!!!!!!!"<<endl<<endl<<endl;
            for (int i=0; i<50; ++i)
                cout<<endl;
}
 
 
int userMain(int argc, char* argv[])
{
    t0 = new TestThread(0);
    t1 = new TestThread(1, t0);
    t2 = new TestThread(2, t1);
    t1->registerHandler(1, handler1);
    t2->registerHandler(2, handler2);
    t0->start();
    t1->start();
    t2->start();
    delete t0;
    delete t1;
    delete t2;
    return 0;
}

