#ifndef task_h
#define task_h

class Task
{
public:
    virtual ~Task() {}
    virtual void begin() = 0;
    virtual void update() = 0;
};

class TaskGroup: public Task
{
public:
    TaskGroup(Task* tasks[], int n);
    void begin();
    void update();

private:
    Task** tasks_;
    int n_;
};

#endif