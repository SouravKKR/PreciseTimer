#include <chrono>
#include "Async/Async.h"
#include "HAL/ThreadSafeBool.h"

UENUM(BlueprintType)
enum TimerInputType
{
	Seconds  UMETA(DisplayName = "Seconds"),
	Milliseconds  UMETA(DisplayName = "Milliseconds"),
	Microseconds UMETA(DisplayName = "MicroSeconds"),
	Nanoseconds UMETA(DisplayName = "NanoSeconds"),
};

DECLARE_DYNAMIC_DELEGATE(FPreciseTimerDelegate);
UCLASS(Blueprintable)
class UPreciseTimer :public UObject
{
	GENERATED_BODY()
	~UPreciseTimer()
	{
		bStop = true;
	}
protected:
	std::chrono::time_point<std::chrono::high_resolution_clock> StartTime, EndTime;
	std::chrono::nanoseconds duration;
	volatile bool bStop = false;
	TimerInputType InType = TimerInputType::Nanoseconds;
public:
	UFUNCTION(BlueprintCallable)
	   void StartTimer()
	{
		 StartTime = std::chrono::high_resolution_clock::now();
		
	}
	void Tick(float Time, bool Looping, FPreciseTimerDelegate Call)
	{
		while (GetTimeElapsed() < Time &&!bStop)
		{

		}
		if (Looping)
		{
			if (bStop)
			{
				ConditionalBeginDestroy();
			}
			else
			{
				if(Call.ExecuteIfBound())
				StartTimerByFunction(Time, Looping, InType, Call);
				else
					ConditionalBeginDestroy();
			}

		}
		else
		{
			Call.Execute();
			ConditionalBeginDestroy();
		}
	}
	UFUNCTION(BlueprintCallable)
		void StartTimerByFunction(float Time, bool Looping,TimerInputType InputType, FPreciseTimerDelegate Call)
	{
		if (InType != InputType)
		{
			InType = InputType;
			switch (InputType)
			{
			case Seconds:Time = Time * 1000000000;
				break;
			case Milliseconds: Time = Time * 1000000;
				break;
			case Microseconds:Time = Time * 1000;
				break;
			}
		}
		StartTimer();
		Async(EAsyncExecution::Thread, [&]()
		{
			Tick(Time, Looping, Call);
		});
	}
	UFUNCTION(BlueprintCallable)
		void StopTimer()
	{
		bStop = true;
		ConditionalBeginDestroy();
	}
	
   UFUNCTION(BlueprintCallable)
	   float GetTimeElapsed()
   {
	   EndTime = std::chrono::high_resolution_clock::now();
	   duration = EndTime - StartTime;
	   
	   return duration.count();
   }
   UFUNCTION(BlueprintPure)
	   float ToSeconds(float Value)
   {
	   return Value / 1000000000;
   }
   UFUNCTION(BlueprintPure)
	   float ToMilliseconds(float Value)
   {
	   return Value / 1000000;
   }
   UFUNCTION(BlueprintPure)
	   float ToMicroseconds(float Value)
   {
	   return Value / 1000;
   }
};
