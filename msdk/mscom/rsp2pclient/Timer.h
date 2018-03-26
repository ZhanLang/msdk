// Timer.h

#ifndef _DWONLOADER_TIMER_H_
#define _DWONLOADER_TIMER_H_

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time.hpp>

namespace downloader
{
    class Timer;
    typedef boost::function<void (void)> TimerHandler;

    class Timer
    {
    public:
        Timer(boost::asio::io_service & ios, unsigned int interval, TimerHandler handler)
            : interval_(interval), handler_(handler), inner_timer_(ios)
        {}

        virtual ~Timer(){stop();}

        void start()
        {
            inner_timer_.expires_from_now(boost::posix_time::milliseconds(interval_));
            inner_timer_.async_wait(boost::bind(&Timer::OnTimerExpired, this, _1));
        }

        void stop()
        {
            inner_timer_.cancel();
        }

        void restart()
        {
            stop();
            start();
        }

        void OnTimerExpired(const boost::system::error_code & ec)
        {
            if (!ec)
            {
                handler_();
            }
        }
        unsigned int interval()
        {
            return interval_;
        }
        unsigned int interval(unsigned int interval)
        {
            unsigned int cur_interval = interval_;
            interval_ = interval;
            return cur_interval;
        }
    
    private:
        unsigned int interval_;
        TimerHandler handler_;
        boost::asio::deadline_timer inner_timer_;
    };

    class PeridoicTimer
        : public Timer
    {
    public:
        PeridoicTimer(boost::asio::io_service & ios, unsigned int interval, TimerHandler handler)
            : Timer(ios, interval, boost::bind(&PeridoicTimer::OnPeridoicTimer, this)), user_handler_(handler)
			, m_status(State_Run)
        {
            start();
        }

		~PeridoicTimer()
		{
			// 当释放的时候可能会多等待 1 个定时器时间.
			m_status = State_Exit;
			while(m_status != State_Destory) Sleep(100);
		}

        // WARNING！有可能会重入
        void OnPeridoicTimer()
        {
			if(m_status == State_Exit) { m_status = State_Destory; return; }

			user_handler_();

			if(m_status == State_Exit) { m_status = State_Destory; return; }

			restart();
        }

    protected:
        TimerHandler user_handler_;

		enum TimerState { State_Run = 0, State_Exit, State_Destory, };
		int m_status;
    };
}
#endif