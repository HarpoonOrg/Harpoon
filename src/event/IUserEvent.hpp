#ifndef IUSEREVENT_H
#define IUSEREVENT_H

#include <cstddef>


class IUserEvent {
public:
	virtual size_t getUserId() const = 0;
};

#endif
