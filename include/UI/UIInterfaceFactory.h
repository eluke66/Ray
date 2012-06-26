#ifndef _UI_INTERFACE_FACTORY_H_
#define _UI_INTERFACE_FACTORY_H_

#include <map>
#include <string>
#include <assert.h>

class UI;

// Base class for creating a UI. Doesn't really do anything, but lets
// our derived classes be templated.
class UICreatorBase {
 public:
  virtual ~UICreatorBase(){}
  virtual UI * createUI( int width, int height ) = 0;
};

// Templated class, allows us to associate a UI type with a particular
// class/object.
template <typename UIType>
class UICreatorImpl : public UICreatorBase {
 public:
  virtual UI * createUI( int width, int height ) {
    return new UIType(width, height);
  }
};

class UIInterfaceFactory {
 public:
  // Main function - creates a UI with the given dimensions with the
  // given name.
  static UI *CreateUserInterface( std::string uiType, int width, int height );

  static inline void AddUICreator( UICreatorBase *creator, std::string uiType ) {
    assert( creator );
    assert( _Creators.find( uiType ) == _Creators.end() );

    // Add the creator for the UIType to the mapping.
    _Creators[ uiType ] = creator;
  }

  static void PrintUITypes();
 private:
  UIInterfaceFactory() {}

  typedef std::map<std::string, UICreatorBase *> UITypeCreatorMap;
  static UITypeCreatorMap _Creators;
};
#endif
