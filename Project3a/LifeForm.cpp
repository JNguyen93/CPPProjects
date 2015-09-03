/* main test simulator */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"

using namespace std;

template <typename T>
void bound(T& x, const T& min, const T& max) {
	assert(min < max);
	if (x > max) { x = max; }
	if (x < min) { x = min; }
}

void LifeForm::eat(SmartPointer<LifeForm> victim){
	if(victim.operator ->()->is_alive){
		energy += (victim.operator ->()->energy * eat_efficiency);
		victim.operator ->()->die();
	}
}

void LifeForm::compute_next_move(){
	double cross_time;
	if(speed == 0)
		cross_time = 100;
	else
		cross_time = this->space.distance_to_edge(pos, course) / speed;
	cross_time += Event::now();
	SmartPointer<LifeForm> self{ this };
	border_cross_event = new Event(cross_time, [self] (void) {self->border_cross();});
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> other){
	bool firstSuccessful, secondSuccessful;
	SmartPointer<LifeForm> self { this };
	auto act1 = encounter(this->info_about_them(other));
	//auto act2 = encounter(other.operator ->()->info_about_them(self));
	if(act1 == LIFEFORM_IGNORE){
		return;
	}
	else {
		auto eat_chance1 = eat_success_chance(this->energy, other.operator ->()->energy);
		auto eat_chance2 = eat_success_chance(other.operator ->()->energy, this->energy);
		if(drand48() < eat_chance1){
			firstSuccessful = true;
		}
		else
			firstSuccessful = false;
		if(drand48() < eat_chance2){
			secondSuccessful = true;
		}
		else
			secondSuccessful = false;

		if(firstSuccessful && secondSuccessful){
			switch(encounter_strategy){
				case(BIG_GUY_WINS) :
						if(this->energy > (other.operator ->()->energy))
							new Event(digestion_time, [self, other] (void) {self->eat(other);});
						else
							new Event(digestion_time, [other, self] (void) {other->eat(other);});
						break;
				case(EVEN_MONEY) :
						if(drand48() >= 0.5){
							new Event(digestion_time, [self, other] (void) {self->eat(other);});
						}
						else{
							new Event(digestion_time, [other, self] (void) {other->eat(self);});
						}
					break;
				case(FASTER_GUY_WINS) :
						if(this->speed > (other.operator ->()->speed))
							new Event(digestion_time, [self, other] (void) {self->eat(other);});
						else
							new Event(digestion_time, [other, self] (void) {other->eat(self);});
					break;
				case(UNDERDOG_IS_HERE) :
						if(this->energy < (other.operator ->()->energy))
							new Event(digestion_time, [self, other] (void) {self->eat(other);});
						else
							new Event(digestion_time, [other, self] (void) {other->eat(self);});
						break;
				case(SLOWER_GUY_WINS) :
						if(this->speed < (other.operator ->()->speed))
							new Event(digestion_time, [self, other] (void) {self->eat(other);});
						else
							new Event(digestion_time, [other, self] (void) {other->eat(self);});
						break;
			}
		}
		else if(!firstSuccessful && !secondSuccessful){
			return;
		}
		else if(firstSuccessful && !secondSuccessful){
			new Event(digestion_time, [self, other] (void) {self->eat(other);});
		}
		else if(!firstSuccessful && secondSuccessful){
			new Event(digestion_time, [other, self] (void) {other->eat(self);});
		}

	}
}

void LifeForm::check_encounter(void){
	auto life = this->space.closest(pos);
	double dist = pos.distance(life->pos);
	SmartPointer<LifeForm> other = SmartPointer<LifeForm>(life);
	if(dist <= encounter_distance){
		this->resolve_encounter(other);
	}
}

void LifeForm::update_position(void){
	double delta_time = update_time - Event::now();
	if(speed > max_speed)
		speed = max_speed;
	double displacement = delta_time * speed;
	double xdist = displacement * cos(course);
	double ydist = displacement * sin(course);
	pos.xpos += xdist;
	pos.ypos += ydist;
	update_time = Event::now();
	energy -= movement_cost(speed, delta_time);
}

void LifeForm::border_cross(void){
	update_position();
	double cross_time;
	if(speed == 0)
		cross_time = 100;
	else
		cross_time = this->space.distance_to_edge(pos, course) / speed;
	cross_time += Event::now();
	SmartPointer<LifeForm> self{ this };
	check_encounter();
	border_cross_event = new Event(cross_time, [self] (void) {self->border_cross();});
}

void LifeForm::region_resize(void){
	border_cross_event->cancel();
	border_cross();
}

void LifeForm::set_course(double newCourse){
	border_cross_event->cancel();
	update_position();
	double cross_time;
	if(speed == 0)
		cross_time = 100;
	else
		cross_time = this->space.distance_to_edge(pos, newCourse) / speed;
	cross_time += Event::now();
	SmartPointer<LifeForm> self{ this };
	border_cross_event = new Event(cross_time, [self] (void) {self->border_cross();});
}

void LifeForm::set_speed(double newSpeed){
	border_cross_event->cancel();
	update_position();
	double cross_time;
	if(newSpeed > max_speed)
		newSpeed = max_speed;
	if(newSpeed == 0)
		cross_time = 100;
	else
		cross_time = this->space.distance_to_edge(pos, course) / newSpeed;
	cross_time += Event::now();
	SmartPointer<LifeForm> self{ this };
	border_cross_event = new Event(cross_time, [self] (void) {self->border_cross();});
}

void LifeForm::reproduce(SmartPointer<LifeForm> child){
	if((reproduce_time + min_reproduce_time) >= Event::now()){
		energy = (energy / 2) * (1.0 - reproduce_cost);
		child->energy = energy;
	}
	else{
		delete child.operator ->();
	}

}

void LifeForm::age(void){
	energy -= age_penalty;
	if(energy < min_energy){
		this->die();
	}
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	new Event(age_frequency, [self] (void) { self->age();});
}

ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
	ObjInfo info;

	info.species = neighbor->species_name();
	info.health = neighbor->health();
	info.distance = pos.distance(neighbor->position());
	info.bearing = pos.bearing(neighbor->position());
	info.their_speed = neighbor->speed;
	info.their_course = neighbor->course;
	return info;
}

ObjList LifeForm::perceive(double radius){
	double range = radius;
	int i;
	if(radius > max_perceive_range)
		range = max_perceive_range;
	if(radius < min_perceive_range)
		range = min_perceive_range;
	energy -= perceive_cost(range);
	std::vector<SmartPointer<LifeForm>> vectlist = this->space.nearby(pos, range);
	ObjList list;
	for(i = 0; i < vectlist.size(); i++){
		list.push_back(info_about_them(vectlist[i]));
	}
	return list;
}

