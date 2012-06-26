#include "UI/UIInterfaceFactory.h"
#include "Core/utils.h"

UIInterfaceFactory::UITypeCreatorMap
UIInterfaceFactory::_Creators;


UI *
UIInterfaceFactory::CreateUserInterface( std::string uiType, int width, int height ) {

  // Find the creator for this UI type
  UITypeCreatorMap::iterator it = _Creators.find( uiType );

  if ( it == _Creators.end() ) {
    ERROR("Unknown UI type " << uiType );
    assert(0);
  }


  // Create the UI
  assert( it->second );
  return it->second->createUI( width, height );
}

void
UIInterfaceFactory::PrintUITypes() {
  for ( UITypeCreatorMap::iterator it = _Creators.begin(); it != _Creators.end(); it++ ) {
    std::cerr << it->first << " ";
  }
  std::cerr << std::endl;
}
