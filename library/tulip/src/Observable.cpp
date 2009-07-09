#include <cassert>
#include <iostream>
#include "tulip/Observable.h"


using namespace std;
using namespace stdext;
using namespace tlp;

#ifdef _WIN32 
#ifdef DLL_EXPORT
int Observable::holdCounter=0;
ObserverMap Observable::holdMap;
#endif
#else
int Observable::holdCounter=0;
ObserverMap Observable::holdMap;
#endif

static bool unholdLock=false;

void Observable::addObserver(Observer *obs) {
  // ensure obs does not already exists in observersList
  slist<Observer*>::iterator itlObs = observersList.begin();
  slist<Observer*>::iterator itle = observersList.end();
  while (itlObs != itle) {
    if (obs == (*itlObs))
      return;
    ++itlObs;
  }  
  observersList.push_front(obs);
}

//===============================================================
void Observable::notifyDestroy() {
  //  cerr << "Observable::notifyObservers" << endl;
  slist<Observer*>::iterator itlObs = observersList.begin();
  slist<Observer*>::iterator itle = observersList.end();
  while (itlObs != itle) {
    Observer* observer = *itlObs;
    // iterator is incremented before
    // to ensure it will not be invalidated
    // during the call to the observableDestroyed method
    ++itlObs;
    observer->observableDestroyed(this);
  }
}
//===============================================================
void Observable::notifyObservers() {
  slist<Observer*>::iterator itlObs = observersList.begin();
  slist<Observer*>::iterator itle = observersList.end();
  if (itlObs == itle)
    return;

  if (unholdLock) {
    cerr << "Cannot notifyObservers during unholdings" << endl;
    return;
  }
  //  cerr << "Observable::notifyObservers" << endl;
  assert(holdCounter>=0);
  if (holdCounter)
    for (;itlObs != itle; ++itlObs)
      holdMap[*itlObs].insert(this);
  else {
    set<Observable *> tmpSet;
    tmpSet.insert(this);
    while(itlObs != itle) {
      Observer* observer = *itlObs;
      // iterator is incremented before
      // to ensure it will not be invalidated
      // during the call to the update method
      ++itlObs;
      observer->update(tmpSet.begin(),tmpSet.end());
    }
  }
}
//===============================================================
void Observable::holdObservers() {
  //  cerr << __PRETTY_FUNCTION__ << " :=> " << holdCounter << endl;
  assert(holdCounter>=0);
  holdCounter++;
}
//===============================================================
void Observable::unholdObservers() {
  assert(holdCounter>=0);
  //  cerr << __PRETTY_FUNCTION__ << " :=> " << holdCounter << endl;
  holdCounter--;
  if (unholdLock) return;
  unholdLock=true; 
  if (holdCounter==0) {
    ObserverMap tmp(holdMap);
    holdMap.clear();
    ObserverMap::iterator itMObs;
    for (itMObs=tmp.begin();itMObs!=tmp.end();++itMObs) {
      itMObs->first->update(itMObs->second.begin(),itMObs->second.end());
    }
  }
  unholdLock=false;
}
//===============================================================
