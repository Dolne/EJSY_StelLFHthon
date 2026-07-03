#include "task.h"

TaskGroup::TaskGroup(Task* tasks[], int n) {
    tasks_ = tasks; //Shallow copy?
    n_ = n;
}

void TaskGroup::begin() { //Begin all the tasks within TaskGroup
    for (int i = 0; i < n_; i++) {
        tasks_[i]->begin();
    }
}

void TaskGroup::update() { //Update all the tasks within TaskGroup
    for (int i = 0; i < n_; i++) {
        tasks_[i]->update();
    }
}