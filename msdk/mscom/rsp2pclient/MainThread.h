#ifndef __MainThread_H__
#define __MainThread_H__


#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>


namespace downloader
{
    class MainThread
    {
    public:
        MainThread()
        {
            ios_ = new boost::asio::io_service();
            work_ = new boost::asio::io_service::work(*ios_);
            thread_ = new boost::thread(boost::bind(&boost::asio::io_service::run, ios_));
        }

        ~MainThread()
        {
            ios_->stop();
            thread_->join();
            delete thread_;
            delete work_;
			delete ios_;
        }

        boost::asio::io_service & ios() {return *ios_;}

        void post(boost::function<void ()> f) {ios_->post(f);}

    private:
        boost::thread * thread_;
        boost::asio::io_service * ios_;
        boost::asio::io_service::work * work_;
    };
}


#endif
