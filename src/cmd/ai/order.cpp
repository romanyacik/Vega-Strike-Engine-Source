/*
 * Vega Strike
 * Copyright (C) 2001-2002 Daniel Horn
 *
 * http://vegastrike.sourceforge.net/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "vegastrike.h"
#include "cmd/unit.h"
#include "order.h"
#include "cmd/collection.h"
#include "communication.h"
#include "config_xml.h"
#include "vs_globals.h"
//#define ORDERDEBUG
void Order::Execute () {
  static float airesptime=XMLSupport::parse_float(vs_config->getVariable ("AI","CommResponseTime","3"));
  ProcessCommunicationMessages(airesptime);
  int completed=0;
  unsigned int i=0;

  for (i=0;i<suborders.size();i++) {
    if ((completed& ((suborders[i])->getType()&(MOVEMENT|FACING|WEAPON)))==0) {
      (suborders[i])->Execute();
      completed|=(suborders[i])->getType();
      if ((suborders[i])->Done()) {
	vector<Order*>::iterator ord = suborders.begin()+i;
	delete (*ord);
	ord =suborders.erase(ord);
	i--;
      } 
    }
  }
  
  if (suborders.size()==0) {
    done = true;
  } else{
    done = false;
  }

  return;
}

Order * Order::queryType (int type) {
  for (unsigned int i=0;i<suborders.size();i++) {
    if ((suborders[i]->type&type)==type) {
      return suborders[i];
    }
  }
  return NULL;
}


void Order::eraseType (int type) {
  for (unsigned int i=0;i<suborders.size();i++) {
    if ((suborders[i]->type&type)==type) {
      delete suborders[i];
      vector <Order *>::iterator j= suborders.begin()+i;
      suborders.erase(j);
      i--;
    }
  }
}

Order* Order::EnqueueOrder (Order *ord) {

  if(ord==NULL){
    printf("NOT ENQEUEING NULL ORDER\n");
    printf("this order: %s\n",getOrderDescription().c_str());
    return NULL;
  }
  ord->SetParent(parent);
  suborders.push_back (ord);
  return this;
}
Order* Order::EnqueueOrderFirst (Order *ord) {
  if(ord==NULL){
    printf("NOT ENQEUEING NULL ORDER\n");
    printf("this order: %s\n",getOrderDescription().c_str());
    return NULL;
  }

  ord->SetParent(parent);

  vector<Order*>::iterator first_elem = suborders.begin();
  suborders.insert (first_elem,ord);
  return this;
}
Order* Order::ReplaceOrder (Order *ord) {
  vector<Order*>::iterator ordd = suborders.begin();
  for (unsigned int i=0;i<suborders.size();i++) {
    if (!(ord->getType()&(*ordd)->getType()&(FACING|WEAPON|MOVEMENT))){
      	delete (*ordd);
	ordd =suborders.erase(ordd);
    } else {
      ordd++;
    }
  }
  suborders.push_back(ord);
  return this;

}

bool Order::AttachOrder (UnitCollection *targets1) {
  if (!(type&TARGET)) {
    if (type&SELF) {
      return AttachSelfOrder (targets1);//can use attach order to do shit
    }
    return false;
  }
  if (targets) {
    delete targets;
  
  }
  targets = new UnitCollection();
  UnitCollection::UnitIterator iter = targets1->createIterator();
  targets->prepend (&iter);
  return true;
}
bool Order::AttachSelfOrder (UnitCollection *targets1) {
  if (!(type&SELF))
    return false;
  if (group)
    delete group;
  if (targets1==NULL) {
    group = NULL;
  } else {
    group = new UnitCollection();
    UnitCollection::UnitIterator iter = targets1->createIterator();
    Unit *u;
    while(0!=(u = iter.current())) {
      group->prepend (u);
      iter.advance();
    }
  }
  return true;
}
bool Order::AttachOrder (Vector targetv) {
  if (!(type&LOCATION)) {
    return false;
  }
  targetlocation = targetv;
  return true;
}

void ExecuteFor::Execute() {
  if (child) {
    child->SetParent(parent);
    type = child->getType();
  }
  if (time>maxtime) {
    done = true;

    return;
  }
  time +=SIMULATION_ATOM;
  if (child)
    child->Execute();
  return;
}

Order* Order::findOrder(Order *ord){
  if(ord==NULL){
    printf("FINDING EMPTY ORDER\n");
    printf("this order: %s\n",getOrderDescription().c_str());
    return NULL;
  }
  for (unsigned int i=0;i<suborders.size();i++) {
    if (suborders[i]==ord) {
      return suborders[i];
    }
  }
  return NULL;
}
Order::~Order () {
  unsigned int i;
  for (i=0;i<suborders.size();i++) {
    if(suborders[i]==NULL){
      printf("ORDER: a null order\n");
      printf("this order: %s\n",getOrderDescription().c_str());
    }
    else{
      delete suborders[i];
    }
  }
  for (i=0;i<messagequeue.size();i++) {
    delete messagequeue[i];
  }
  messagequeue.clear();
  suborders.clear();
}
void Order::ClearMessages() {
  unsigned int i;
  for (i=0;i<suborders.size();i++) {
    suborders[i]->ClearMessages();
  }
  for (i=0;i<messagequeue.size();i++) {
    delete messagequeue[i];
  }
  messagequeue.clear();
}
void Order::eraseOrder(Order *ord){
  bool found=false;
  if(ord==NULL){
    printf("NOT ERASING A NULL ORDER\n");
    printf("this order: %s\n",getOrderDescription().c_str());
    return;
  }

  for (unsigned int i=0;i<suborders.size() && found==false;i++) {
    if (suborders[i]==ord){
      delete suborders[i];
      vector <Order *>::iterator j= suborders.begin()+i;
      suborders.erase(j);
      found=true;
      //      printf("erased order\n");
    }
  }

  if(!found){
    printf("TOLD TO ERASE AN ORDER - NOT FOUND\n");
    printf("this order: %s\n",getOrderDescription().c_str());

  }
}

Order* Order::findOrderList(){
  olist_t *orderlist=getOrderList();

  if(orderlist){
    return this;
  }

  Order *found_order=NULL;
  for (unsigned int i=0;i<suborders.size() && found_order==NULL;i++) {
    found_order=suborders[i]->findOrderList();
  }

  return found_order;
}

string Order::createFullOrderDescription(int level){
  string tabs;
  for(int i=0;i<level;i++){
    tabs=tabs+"   ";
  }

  string desc=tabs+"+"+getOrderDescription()+"\n";

  for (unsigned int j=0;j<suborders.size();j++) {
    desc=desc+suborders[j]->createFullOrderDescription(level+1);
  }

  return desc;
}
