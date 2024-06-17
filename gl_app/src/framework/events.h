#pragma once

#include "base.h"

/*
	//strongly typed - need to cast to (size_t) to use as ints
	enum class Type : int32_t
	{
	kNone, kMouseMove, kMouseLDown, kMouseLUp, kMouseRDown, kMouseRUp,
	kMouseScroll, kKeyPressed, kKeyReleased, kWinResize, kWinMove, kCount
	};
	*/

struct Event
{
public:
	enum
	{
		kAny, 
		kMouseMove, 
		kMouseLDown, 
		kMouseLUp, 
		kMouseRDown, 
		kMouseRUp, 
		kMouseMDown, 
		kMouseMUp,
		kMouseScroll, 
		kKeyPressed, 
		kKeyReleased, 
		kWinResize, 
		kWinMove, 
		kCount
	};
	virtual ~Event() = default;
	virtual int32_t type() { return kAny; }
	static int32_t count() { return kCount; }

private:
	bool handled = false;
};

struct EventKeyPressed : public Event
{
	EventKeyPressed() : key{ 0 } {}
	EventKeyPressed(int32_t _key) : key{_key} {}

	int32_t key;

	int32_t type() override { return Event::kKeyPressed; }
	static uint32_t static_type() { return Event::kKeyPressed; }
};

struct EventKeyReleased : public Event
{
	EventKeyReleased() : key{ 0 } {}
	EventKeyReleased(int32_t _key) : key{_key} {}

	int32_t key;

	int32_t type() override { return Event::kKeyReleased; }
	static uint32_t static_type() { return Event::kKeyReleased; }
};


struct EventMouseMove : public Event
{
	EventMouseMove() : x{ 0 }, y{ 0 }, delta_x{ 0 }, delta_y{ 0 } {}
	EventMouseMove(float _x, float _y, float _delta_x, float _delta_y) :
		//Initializer list doesn't work when there are virtual functions!?
		x{ _x }, y{_y}, delta_x {_delta_x}, delta_y {_delta_y}
	{}

	float x, y;
	float delta_x, delta_y;

	int32_t type() override { return Event::kMouseMove; }
	static uint32_t static_type() { return Event::kMouseMove; }
};

struct EventMouseScroll : public Event
{
	EventMouseScroll() : x_offset{ 0 }, y_offset{ 0 } {}
	EventMouseScroll(float _x_offset, float _y_offset) :
		x_offset{ _x_offset }, y_offset{ _y_offset }
	{}

	float x_offset, y_offset;

	int32_t type() override { return Event::kMouseScroll; }
	static uint32_t static_type() { return Event::kMouseScroll; }
};

struct EventMouseLDown : public Event
{
	EventMouseLDown() : x{ 0 }, y{ 0 }{}
	EventMouseLDown(float _x, float _y) : x{ _x }, y{ _y } {}

	float x, y;
	int32_t type() override { return Event::kMouseLDown; }
	static uint32_t static_type() { return Event::kMouseLDown; }
};

struct EventMouseLUp : public Event
{
	EventMouseLUp() : x{ 0 }, y{ 0 }{}
	EventMouseLUp(float _x, float _y) : x{ _x }, y{ _y } {}

	float x, y;
	int32_t type() override { return Event::kMouseLUp; }
	static uint32_t static_type() { return Event::kMouseLUp; }
};

struct EventMouseRDown : public Event
{
	EventMouseRDown() : x{ 0 }, y{ 0 }{}
	EventMouseRDown(float _x, float _y) : x{ _x }, y{ _y } {}

	float x, y;
	int32_t type() override { return Event::kMouseRDown; }
	static uint32_t static_type() { return Event::kMouseRDown; }
};

struct EventMouseRUp : public Event
{
	EventMouseRUp() : x{ 0 }, y{ 0 }{}
	EventMouseRUp(float _x, float _y) : x{ _x }, y{ _y } {}

	float x, y;
	int32_t type() override { return Event::kMouseRUp; }
	static uint32_t static_type() { return Event::kMouseRUp; }
};

struct EventMouseMDown : public Event
{
	EventMouseMDown() : x{ 0 }, y{ 0 } {}
	EventMouseMDown(float _x, float _y) : x{ _x }, y{ _y } {}

	float x, y;
	int32_t type() override { return Event::kMouseMDown; }
	static uint32_t static_type() { return Event::kMouseMDown; }
};

struct EventMouseMUp : public Event
{
	EventMouseMUp() : x{ 0 }, y{ 0 } {}
	EventMouseMUp(float _x, float _y) : x{ _x }, y{ _y } {}

	float x, y;
	int32_t type() override { return Event::kMouseMUp; }
	static uint32_t static_type() { return Event::kMouseMUp; }
};

struct EventWinResize : public Event
{
	EventWinResize() : buffer_width{ 0 }, buffer_height{ 0 }{}
	EventWinResize(int32_t _buffer_width, int32_t _buffer_height) :
		buffer_width{ _buffer_width }, buffer_height{ _buffer_height } {}

	int32_t buffer_width, buffer_height;

	int32_t type() override { return Event::kWinResize; }
	static uint32_t static_type() { return Event::kWinResize; }
};

template<typename E> using CallbackFn = std::function<void(E&)>;
#define CALLBACK_TYPEDECL(e) std::function<void(e&)> //not used
#define CALLBACK_TYPEDECL_2(t,e,fn_name) void (t::* fn_name)(e&) //not used

class IEventCallback
{
public:
	virtual ~IEventCallback() = default;
	virtual void Dispatch() = 0;
	virtual void SetEvent(Event& event) = 0;
};


template<typename E>
class EventCallback : public IEventCallback
{
	//Use std::function
public:
	EventCallback(CallbackFn<E>& callback_fn) : m_callback_fn{ callback_fn } {}
	void SetEvent(Event& event) override { m_event = static_cast<E&>(event); }
	void Dispatch() override { m_callback_fn(m_event); }

private:
	E m_event;
	CallbackFn<E> m_callback_fn;
};

template<typename T, typename E>
class EventCallback2 : public IEventCallback
{
	//Use pointer to a class member function
public:
	EventCallback2(T* instance, void (T::* callback_fn)(E&)) : m_instance(instance), m_callback_fn(callback_fn) {}
	void SetEvent(Event& event) override { m_event = static_cast<E&>(event); }
	void Dispatch() override { (m_instance->*m_callback_fn)(m_event); } //Think the '*' is optional
private:
	E m_event;
	void (T::* m_callback_fn)(E&);
	T* m_instance;
};


class EventManager
{
public:
	EventManager(EventManager& other) = delete;
	EventManager& operator = (EventManager& other) = delete;
	~EventManager() = default;

	template<typename T, typename E>
	static void SetCallback(T* instance, void (T::* callback_fn)(E&))
	{
		IEventCallback* callback = EventManager::MakeCallback(instance, callback_fn);
		s_instance.m_callback_list[E::static_type()] = callback;
	}

	template<typename E>
	static void Dispatch(E& event)
	{
		ASSERT(s_instance.m_callback_list[E::static_type()] != nullptr);

		s_instance.m_callback_list[E::static_type()]->SetEvent(event);
		s_instance.m_callback_list[E::static_type()]->Dispatch();
	}

	static EventManager& instance() { return s_instance; }
	
private:

	template<typename T, typename E>
	static IEventCallback* MakeCallback(T* instance, void (T::* callback_fn)(E&))
	{
		//both of these work!

		//option 1
		CallbackFn<E> fn = std::bind(callback_fn, instance, std::placeholders::_1);
		IEventCallback* opt1 = new EventCallback<E>(fn);

		//option 2
		IEventCallback* opt2 = new EventCallback2<T, E>(instance, callback_fn);

		return opt2;
	}

	EventManager()
	{
		IEventCallback* default_callback = EventManager::MakeCallback(this, &EventManager::DefaultCallBack);
		for (uint32_t i = 0; i < m_callback_list.size(); i++)
		{	
			if (m_callback_list[i] == nullptr)
			{
				m_callback_list[i] = default_callback;
			}
		}
	}
	void DefaultCallBack(Event& e)
	{
		//std::cout << "Default event callback\n";
	}

	std::array<IEventCallback*, Event::kCount> m_callback_list;
	static EventManager s_instance;
};

