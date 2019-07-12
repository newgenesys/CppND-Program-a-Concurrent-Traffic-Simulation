#include <iostream>
#include <random>
#include <future>
#include "TrafficLight.h"

#include <chrono>
#include <thread>

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
	
	// perform queue modification under the lock
	std::unique_lock<std::mutex> uLock(_mutex);
	_condition.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

	// remove last vector element from queue
	T msg = std::move(_queue.back());
	_queue.pop_back();

    return msg; // will not be copied due to return value optimization (RVO) in C++
	
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.            
        
        // simulate some work
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);

        // add vector to queue
        std::cout << "   Message " << msg << " has been sent to the queue" << std::endl;
        _queue.push_back(std::move(msg));
        _condition.notify_one(); // notify client after pushing new Vehicle into vector
    
}


/* Implementation of class "TrafficLight" */

/*
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}
*/
void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    //queue (new MessageQueue<TrafficLightPhase>);
    
    while(true){
		//std::cout << "WaitingForGreen: ";
		TrafficLightPhase message = queue->receive();//.getCurrentPhase()
		//std::cout << "Message: " << message << "\n";
		//std::cout << "Message == : " << TrafficLightPhase::green << "\n";
		if(message == TrafficLightPhase::green){
			return;
		}
    }
}


TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class.
    // launch cycleThroughPhases function in a thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this)); 
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    
	//time_t start, end; 
	//auto start = std::chrono::high_resolution_clock::now(); 
	//auto stop = std::chrono::high_resolution_clock::now();
    while(true){
		//time(&start); //
		//start = std::chrono::high_resolution_clock::now(); 
		if(loopCount == 2){									
			loopCount = 0;
			TrafficLight::delayT(0.001);
			
			//double time_taken = double(end - start);//
			//std::chrono::duration<double, std::milli> elapsed = stop-start; 
			//stop = std::chrono::high_resolution_clock::now();
			//std::cout << "2 Loops Cycles took " << elapsed.count()/1000 << " seconds\n";
			
			if(_currentPhase == TrafficLightPhase::red){
				_currentPhase = TrafficLightPhase::green;
			}else{
				_currentPhase = TrafficLightPhase::red;
			}
			
			TrafficLightPhase message = _currentPhase;
			std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, queue, std::move(message));		
		}
		
		loopCount+=1;	
		int rval = (rand() % 6) + 4;
		TrafficLight::delayT(rval);
		//time(&end);//
			
    }
}

void TrafficLight::delayT(int d){	
	using namespace std::chrono_literals;
	std::cout << "Waiting ...\n" << std::flush;
	auto start = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::seconds(d));
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end-start;
	std::cout << "Done Waiting! " << elapsed.count() << " ms\n";	
}
