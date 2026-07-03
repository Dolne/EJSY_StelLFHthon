#ifndef task_h
#define task_h //Prevents double definition

class Task //Create abstract base class, must be derived from, Task cannot be created directly
{
public:
    virtual ~Task() {}
    virtual void begin() = 0;  //Anything that inherits from Task must implement begin() and update()
    virtual void update() = 0;
};

class TaskGroup: public Task //Composite Design Pattern: a TaskGroup is itself a Task, while also containing many other Tasks. That lets you treat a whole group exactly like a single task.
{
public:
    TaskGroup(Task* tasks[], int n); //Array of pointers to Task objects and length of array
    void begin();
    void update();

private:
    Task** tasks_; //Adress of first element of array of Task objects
    int n_;
};

#endif