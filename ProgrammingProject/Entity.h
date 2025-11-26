#ifndef ENTITY_H
#define ENTITY_H
#include <iostream>
#include <string>

class Entity {
protected:
	std::string fID;
	std::string fName;
public:
	Entity();
	Entity(const std::string& n);
	Entity(const std::string& id, const std::string& n);
	virtual ~Entity();
	virtual std::string getID() const;
	virtual std::string getName() const;
	virtual void displayInformation();
};

#endif /* ENTITY_H */