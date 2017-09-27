
#ifndef _Observer_H_
#define _Observer_H_

#include <vector>

template <class NotificationTypeT>
class Observable;

template <class NotificationTypeT>
class Observer {
public:
	virtual bool observableUpdate(NotificationTypeT notification, Observable<NotificationTypeT>* src) = 0;
	virtual void observableRevoke(NotificationTypeT notification, Observable<NotificationTypeT>* src) = 0;
};

template <class NotificationTypeT>
class Observable {
public:
	void attach(Observer<NotificationTypeT> *obs);
	void detach(Observer<NotificationTypeT> *obs);
	bool notify(NotificationTypeT notification);
protected:
	void revoke(NotificationTypeT notification);
private:
	std::vector<Observer<NotificationTypeT>*> _views;
};

template <class NotificationTypeT>
class ObservablePropagator : public Observable<NotificationTypeT>, public Observer<NotificationTypeT> {
public:
	bool observableUpdate(NotificationTypeT notification, Observable<NotificationTypeT>* src) override;
	void observableRevoke(NotificationTypeT notification, Observable<NotificationTypeT>* src) override;
private:
};

#include "Observer.inl"
#endif