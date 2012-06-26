#include "IO/ConfigFileParser.h"
#include "Core/World.h"
#include "IO/ObjectLoader.h"
#include <fstream>
#include "Core/Camera.h"
#include "Property/Property.h"
#include "Property/PropertyFactory.h"
#include "Samplers/Sampler.h"

extern std::string OutputFilename;

ConfigFileParser
ConfigFileParser::_parser;

const std::string EOC = "eoc";

extern void SetGlossConfig( const SamplerConfig &config );

void ConfigFileParser::ParseFile( std::string filename, bool saveStream ) {

  if ( !_parser._initializedReader ) {
    _parser.initialize( filename.empty() );
    _parser._initializedReader = true;
  }

  // If the filename is empty, just read from stdin
  if ( filename.empty() ) {
    _parser.processIstream( std::cin, true );
  }
  else {
    std::ifstream infile( filename.c_str() );
    
    if ( !infile.is_open() ) {
      std::cerr << "Unable to open config file " << filename << std::endl;
      return;
    }
    _parser.processIstream( infile );
    
    if ( saveStream )
      _parser.saveStream( infile );
    
    infile.close();
  }
}

void
ConfigFileParser::saveStream( std::istream &infile ) {

  std::string input;
  
  infile.clear();
  infile.seekg(0, std::ios::beg);
  while ( infile >> input ) {
    // Don't read comments
    if ( input[0] == '#' ) {
      infile.getline( _buffer, BUFFER_SIZE );
      continue;
    }
    
    if ( input == "include" ) {
      std::string file;
      infile >> file;
      std::ifstream nextinfile( file.c_str() );
      
      if ( !nextinfile.is_open() ) {
	std::cerr << "Unable to open config file " << file << std::endl;
	return;
      }
      saveStream( nextinfile );
      nextinfile.close();
    }
    else {
      _savedStream << input << " ";
    }
  }

}

std::string ConfigFileParser::GetStream() {
  return _parser._savedStream.str();
}

void ConfigFileParser::processIstream( std::istream &infile,
				       bool breakOnEOC ) {
  
  std::string input;
  while ( infile >> input ) {
    
    // Don't read comments
    if ( input[0] == '#' ) {
      infile.getline( _buffer, BUFFER_SIZE );
      continue;
    }
    
    // Convert it to lower case
    toLower(input);

    if ( breakOnEOC && input == EOC )
      break;
    
    // Figure out what to do
    TermMap::iterator reader = _inputs.find( input );
    if ( reader != _inputs.end() )
      (*(reader->second))(input, infile);
    else
      std::cerr << "Unknown input -" << input << "-" << std::endl;

  }

  
}



#define DECL(str,type) _inputs[ #str ] = do ## type
#define DECLOTHER(str) DECL(str,Other)
#define DECLTERM(str) DECL(str,Term)
#define DECLVIEW(str) DECL(str,View)
#define DECLDEF(str) DECL(str,Def)
#define DECLOBJ(str) DECL(str,Object)

void ConfigFileParser::initialize( bool slaveMode ) {
  DECLOTHER(outfile);
  DECLOTHER(partitioner);
  DECLOTHER(background);
  DECLOTHER(include);

  if ( slaveMode ) {
    DECLOTHER(eoc);
  }
  
  DECLTERM(ka);
  DECLTERM(kd);
  DECLTERM(ks);
  DECLTERM(kt);
  DECLTERM(exp);
  DECLTERM(atmos);
  DECLTERM(gloss);
  
  DECLVIEW( camera );

  DECLDEF(add);
  DECLDEF(define);
  DECLDEF(defsurface);
  
  DECLOBJ(sphere);
  DECLOBJ(plane);
  DECLOBJ(polygon);
  DECLOBJ(triangle);
  DECLOBJ(object);
  DECLOBJ(cone);
  DECLOBJ(light);
  DECLOBJ(disk);
  DECLOBJ(ring);
  DECLOBJ(cylinder);
}

bool ConfigFileParser::doOther(  std::string input, std::istream &infile ) {
  
  if ( input == "outfile" )
    infile >> OutputFilename;
  else if ( input == "partitioner" ) {
    std::string part;
    infile >> part; toLower(part);
    World::Instance()->setPartitionerType( SpacePartitionerFactory::StringToType(part) );
  }
  else if ( input == "background" ) {
    std::string bg;

    DEBUG("Setting world background" );
    Surface * surface = new Surface;
    infile >> surface;

    World::Instance()->setBackground( surface );
  }
  else if ( input == "include" ) {
    std::string file;
    infile >> file;
    if (!g_quietMode)
      std::cerr << "Processing file " << file << std::endl;
    _parser.ParseFile( file );
  }
  else if ( input == "eoc" ) {
    
  }
  else return false;

  return true;
  
}

bool ConfigFileParser::doTerm(   std::string input, std::istream &infile ){
  ShadingTerms &terms = World::Instance()->shadingTerms();
  
  if ( input == "ka" )
    infile >> terms._ka;
  else if ( input == "kd" )
    infile >> terms._kd;
  else if ( input == "ks" )
    infile >> terms._ks;
  else if ( input == "kt" )
    infile >> terms._kt;
  else if ( input == "exp" )
    infile >> terms._exp;
  else if ( input == "atmos" )
    infile >> terms._atmos2
	   >> terms._atmos1;
  else if ( input == "gloss" ) {
    SamplerConfig config;
    infile >> config;
    SetGlossConfig(config);
  }
    
  else
    return false;

  return true;
}

bool ConfigFileParser::doView(   std::string input, std::istream &infile ){
  Point p;
  char brace;
  std::string otherInput;
  infile >> brace;

  READLN( otherInput, infile );

  Camera * camera = NULL;
  if ( otherInput == "pinhole" ) {
    camera = new PinholeCamera();
    double d;
    infile >> d;
    camera->hfov( d );
  }
  else {
    ERROR("Unknown type of camera " << otherInput );
    assert(0);
  }

  READLN( otherInput, infile );
  while ( otherInput != "}" ) {
    if (otherInput ==  "eye") {
      infile >> p;
      camera->eye(p);
    }
    else if ( otherInput == "at" ) {
      infile >> p;
      camera->at(p);
    }
    else if ( otherInput == "up" ) {
      infile >> p;
      camera->up((Vector)p);
    }
    else {
      ERROR("Unknown camera input " << otherInput );
    }
    READLN( otherInput, infile );
  } // End of while

  World::Instance()->camera( camera );
  return true;
}

bool ConfigFileParser::doDef(    std::string input, std::istream &infile ){
  if ( input == "define" ) {
    // Get name
    std::string name, inputString, tmp;
    infile >> name;
    char brace;
    infile >> brace;

    int numBraces = 1;
    
    while (1) {
      infile >> tmp;
      if ( tmp == "{" )
	numBraces++;
      if ( tmp == "}" ) {
	numBraces--;
	if ( numBraces == 0 )
	  break;
      }
      if ( tmp[0] == '#' ) {
	infile.getline( _parser._buffer, BUFFER_SIZE );
	continue;
      }
      inputString += tmp + " ";
    }

    _parser._objectDefinitions[ name ] = inputString;
    DEBUG( "Definition for " << name << " is " << inputString );
  }
  else if ( input == "defsurface" ) {
    // Get name
    std::string name;
    infile >> name;
    toLower(name);
    
    Surface * newSurface = new Surface;
    infile >> newSurface;
    Surface::AddNamedSurface( name, newSurface );
  }
  else if ( input == "add" ) {
    // Get name to add
    std::string name;
    infile >> name;
    if ( !g_quietMode )
      std::cerr << "Adding " << name << std::endl;
    std::string input;
    //    infile >> input; // 'DONE'
    READLN(input, infile);

    ObjectDefinitionMap::iterator theObject;
    theObject = _parser._objectDefinitions.find( name );
    if ( theObject == _parser._objectDefinitions.end() ) {
      std::cerr << "Undefined object " << name << "!" << std::endl;
      return true;
    }


    std::string currentDefinition = theObject->second;
    std::string newDefinition;
    int numbraces = 0;
    while ( input != "done" ) {
      if ( input == "{" ) {
	if (numbraces != 0 )
	  newDefinition += input + " ";
	numbraces++;
      }
      else if ( input == "}" ) {
	numbraces--;
	if (numbraces != 0 )
	  newDefinition += input + " ";
      }
      else {
	newDefinition += input + " ";
      }
      READLN(input, infile);
    } // input != 'DONE'

    insertDefinition( newDefinition, currentDefinition );

    std::istringstream istr( currentDefinition );
    istr.sync();
    _parser.processIstream( istr );
  }
  else
    return false;
  
  return true;
}

void ConfigFileParser::insertDefinition( const std::string newDefinition,
					 std::string &currentDefinition ) {

  std::istringstream tmpInput( currentDefinition );
  std::string tmp;
  std::string finalString;
  
  // We have two possibilities:
  tmpInput >> tmp;
  finalString += tmp + " ";
  
  if ( tmp == "object" ) {
    // 1. Object. We insert the new text just before the closing '}' at
    // the end of the object definition
    int numbraces = 0;
    tmpInput >> tmp;
    if ( tmp != "{" ) {
      ERROR("Unable to parse additional object info for " << newDefinition );
      return;
    }
    numbraces++;
    finalString += tmp + " ";
    
    while ( numbraces > 0 ) {
      tmpInput >> tmp;
      if ( tmp == "{" )
	numbraces++;
      else if ( tmp == "}" )
	numbraces--;
      if ( numbraces == 0 )
	break;
      finalString += tmp + " ";
    }

    // add in the new text
    finalString += newDefinition + " " + tmp + " ";
  }
  else {
    // 2. Non-object. We insert the new text before the 'end' word.
    tmpInput >> tmp;
    finalString += tmp + " ";
    
    while ( tmp != "end" ) {
      tmpInput >> tmp;
      if  (tmp == "end" )
	break;
      finalString += tmp + " ";
    }

    // add in the new text
    finalString += newDefinition + " " + tmp + " ";
  }

  currentDefinition = finalString;
}

bool ConfigFileParser::doObject( std::string input, std::istream &infile ){

  GeometryType geom = Geometry::FindType( input );
  if ( geom != NO_GEOMETRY ) {
    Object * object = Object::MakeObject( geom, infile );
    World::Instance()->addObject( object );
  }
  else if ( input == "object" ) {
    Object * object = ObjectBuilder::BuildObject( infile );
    World::Instance()->addObject( object );
  }
  else if ( input == "light" ) {
    std::string type;
    Color c;
    Point p;
    Light * light = NULL;
    
    infile >> type >> c;
    toLower(type);
    if ( type == "point" ) {
      infile >> p;
      PointLightInfo pli( c, p );
      light = LightFactory::makeLight( POINT, &pli );
    }
    else if ( type == "area" ) {
      int numSamples;
      infile >> numSamples;
      infile >> type; // "sphere, etc."
      Geometry *lightGeom = GeometryFactory::MakeGeometry( type );
      if ( lightGeom ) {
	infile >> *lightGeom;

	AreaLightInfo pli( c, numSamples, *lightGeom );
	light = LightFactory::makeLight( AREA, &pli );

	// Add the light's geometry to the scene.
	LightSurface * surface = new LightSurface;
	Property * color =
	  PropertyFactory::MakeProperty( Property::LookupType("color"),
					 Property::CONSTANT );
	assert(color);
	ColorConstantProperty * cccp = dynamic_cast<ColorConstantProperty *>( color );
	assert( cccp );
	cccp->setValue( c );
	surface->setProperty( COLOR, color );
#ifdef ERIC_THIS_BREAKS
	Object * object = new Object( surface, lightGeom );

	World::Instance()->addObject( object );
#endif
      }
      else
	ERROR("Not making light for type " << type);
    }
    else if ( type == "directional" ) {
      infile >> p;
      DirectionalLightInfo dli( c, (Vector)p );
      light = LightFactory::makeLight( DIRECTIONAL, &dli );
    }
    else if ( type == "warn" ) {
      Vector direction;
      float falloff;
      infile >> direction >> p >> falloff;
      WarnLightInfo wli( c, direction, p, falloff );
      light = LightFactory::makeLight( WARN, &wli );
    }
    else {
      std::cerr << "Unknown light type " << type << std::endl;
      return false;
    }
    World::Instance()->lights().push_back( light );
  }
  else
    return false;
    
  return true;

}
