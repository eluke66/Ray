#ifndef _CONFIG_FILE_PARSER_H_
#define _CONFIG_FILE_PARSER_H_

#include <string>
#include <istream>
#include <map>
#include "Core/utils.h"

class ConfigFileParser {
public:
  static void ParseFile( std::string filename, bool saveStream=false );

  static std::string GetStream();
    
  
private:
  static ConfigFileParser _parser;
  bool _initializedReader;
  std::ostringstream _savedStream;


  struct OptionParser {
  };
  
  typedef bool (*processFunction)(std::string input, std::istream &infile);
  typedef std::map<std::string, processFunction> TermMap;
  TermMap _inputs;

  typedef std::map<std::string, std::string> ObjectDefinitionMap;
  ObjectDefinitionMap _objectDefinitions;

  
  char _buffer[ BUFFER_SIZE ];
  
  ConfigFileParser() : _initializedReader( false ) {} 
  void processIstream( std::istream &infile, bool breakOnEOC=false );
  void initialize( bool slaveMode );
  void saveStream( std::istream &infile );
  
  static bool doOther(  std::string input, std::istream &infile );
  static bool doTerm(   std::string input, std::istream &infile );
  static bool doView(   std::string input, std::istream &infile );
  static bool doDef(    std::string input, std::istream &infile );
  static bool doObject( std::string input, std::istream &infile );

  static void insertDefinition( const std::string newDefinition,
				std::string &currentDefinition );
};

#endif
