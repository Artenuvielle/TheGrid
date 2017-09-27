#include <algorithm>

template <class NotificationTypeT> inline
bool Observable<NotificationTypeT>::notify(NotificationTypeT notification) {
	bool ret = true;
	for (int i = 0; i < _views.size(); i++) {
		ret = _views[i]->observableUpdate(notification, this) && ret;
	}
	if (!ret) {
		revoke(notification);
	}
	return ret;
}

template <class NotificationTypeT> inline
void Observable<NotificationTypeT>::revoke(NotificationTypeT notification) {
	for (int i = 0; i < _views.size(); i++) {
		_views[i]->observableRevoke(notification, this);
	}
}

template <class NotificationTypeT> inline
void Observable<NotificationTypeT>::attach(Observer<NotificationTypeT> *obs) {
	_views.push_back(obs);
}

template <class NotificationTypeT> inline
void Observable<NotificationTypeT>::detach(Observer<NotificationTypeT> *obs) {
	std::vector<Observer<NotificationTypeT>*>::iterator position = std::find(_views.begin(), _views.end(), obs);
	if (position != _views.end()) {
		_views.erase(position);
	}
}

template <class NotificationTypeT> inline
bool ObservablePropagator<NotificationTypeT>::observableUpdate(NotificationTypeT notification, Observable<NotificationTypeT>* src) {
	return this->notify(notification);
}

template <class NotificationTypeT> inline
void ObservablePropagator<NotificationTypeT>::observableRevoke(NotificationTypeT notification, Observable<NotificationTypeT>* src) {
	this->revoke(notification);
}