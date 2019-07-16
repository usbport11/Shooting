#ifndef timerH
#define timerH

struct stTimer
{
	HWND hWnd;
	unsigned int Id;
	unsigned int Value;
	TIMERPROC TimerProc;
	bool Enabled;
	
	bool Create(HWND inhWnd, unsigned int inId, unsigned int inValue, TIMERPROC inTimerProc=NULL)
	{
		if(!Set(inhWnd, inId, inValue, inTimerProc)) return false;
		return Start();
	}
	
	bool Update(unsigned int inValue)
	{
		if(inValue < 10 || inValue > 60000)
		{
			LogFile<<"Can not create timer. Wrong value (10 - 60000)"<<endl;
			return false;
		}
		if(Value == inValue) return true;
		Value = inValue;
		LogFile<<"Trying update timer: "<<Id<<" value: "<<Value<<endl;
		
		return SetTimer(hWnd, Id, Value, TimerProc);
	}
	
	bool Set(HWND inhWnd, unsigned int inId, unsigned int inValue, TIMERPROC inTimerProc=NULL)
	{
		if(!inhWnd && !inTimerProc)
		{
			LogFile<<"Can not create timer. Empty HWND and TimerProc"<<endl;
			return false;
		}
		if(inValue < 10 || inValue > 60000)
		{
			LogFile<<"Can not create timer. Wrong value (10 - 60000)"<<endl;
			return false;
		}
		hWnd = inhWnd;
		TimerProc = inTimerProc;
		Value = inValue;
		Id = inId;
		
		return true;
	}
	
	bool Start()
	{
		//without HWND timer id is generating, even if id was sended it will be ignored
		if(!hWnd)
		{
			Id = SetTimer(NULL, 0, Value, TimerProc);
			if(!Id)
			{
				LogFile<<"Some error ocurred"<<endl;
				return false;
			}
		}
		else
		{
			if(!SetTimer(hWnd, Id, Value, TimerProc))
			{
				LogFile<<"Some error ocurred"<<endl;
				return false;
			}
		}
		LogFile<<"Start timer: "<<Id<<endl;
		
		return true;
	}
	
	void Stop()
	{
		LogFile<<"Stop timer: "<<Id<<endl;
		KillTimer(hWnd, Id);
	}
};

#endif
