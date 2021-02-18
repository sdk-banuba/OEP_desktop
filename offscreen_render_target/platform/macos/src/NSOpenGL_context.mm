#import "main_queue_runner_OChelper.h"

@implementation main_queue_runner_OChelper

void main_queue_runner::run_on_main_queue(std::function<void()> f)
{
    if ([NSThread isMainThread])
    {
        f();
    }
    else
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            f(); 
        });
    }
}

@end
