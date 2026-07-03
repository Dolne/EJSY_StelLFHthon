#include "task.h"

TaskGroup::TaskGroup(Task* tasks[], int n) {
    tasks_ = tasks;
    n_ = n;
}

void TaskGroup::begin() {
    for (int i = 0; i < n_; i++) {
        tasks_[i]->begin();
    }
}

void TaskGroup::update() {
    for (int i = 0; i < n_; i++) {
        tasks_[i]->update();
    }
}