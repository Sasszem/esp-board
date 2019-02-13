
#include <vector>
typedef struct {
    void (*task)();
    unsigned int interval;
    unsigned int target;
} Task;

class TaskManager
{
  public:
    std::vector<Task> tasks;
    void update(unsigned int t);
    void add_task(void (*task)(), unsigned int interval, unsigned int t); 
    int task_count=0;
};
