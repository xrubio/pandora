#!/usr/bin/env python3
import sys, os


def writeRegisterTypes( f, listAgents ):
    f.write('void MpiFactory::registerTypes()\n')
    f.write('{\n')  
    for i in range(0, len(listAgents)):
        f.write('\t_types.insert( std::make_pair( "'+listAgents[i]+'", create'+listAgents[i]+'Type()));\n')
    f.write('}\n')
    f.write('\n')

def writeCreateDefaultPackage( f, listAgents ):
    f.write('void * MpiFactory::createDefaultPackage( const std::string & type )\n')
    f.write('{\n')
    for i in range(0, len(listAgents)):
        f.write('\tif(type.compare("'+listAgents[i]+'")==0)\n')
        f.write('\t{\n')
        f.write('\t\treturn new '+listAgents[i]+'Package;\n')
        f.write('\t}\n')
    f.write('\n')
    f.write('\tstd::stringstream oss;\n')
    f.write('\toss << "MpiFactory::createDefaultPackage - unknown agent type: " << type;\n')
    f.write('\tthrow Engine::Exception(oss.str());\n')
    f.write('\treturn 0;\n')
    f.write('}\n')
    f.write('\n')
    return None

def writeCreateAndFillAgents( f, listAgents, namespaces ):
    f.write('Agent * MpiFactory::createAndFillAgent( const std::string & type, void * package )\n')
    f.write('{\n')
    for i in range(0, len(listAgents)):
        f.write('\tif(type.compare("'+listAgents[i]+'")==0)\n')
        f.write('\t{\n')
        f.write('\t\treturn new '+namespaces[i]+"::"+listAgents[i]+'(package);\n')
        f.write('\t}\n')
    f.write('\n')
    f.write('\tstd::stringstream oss;\n')
    f.write('\toss << "MpiFactory::createDefaultPackage - unknown agent type: " << type;\n')
    f.write('\tthrow Engine::Exception(oss.str());\n')
    f.write('\treturn 0;\n')


    f.write('}\n')
    f.write('\n')
    return None

def getMpiTypeAttribute( typeAttribute ):
    mpiTypeAttribute = 'MPI_INT'
    if(typeAttribute=='float'):
        mpiTypeAttribute = 'MPI_FLOAT'
    return mpiTypeAttribute

def writeCreateType( f, nameAgent, attributesMap ):
    # we have to send 4 basic attributes (_id, _position._x, _position._y & _exists + the number of dynamic attributes
    numAttributes = 4+len(attributesMap)
    f.write('MPI_Datatype * create'+nameAgent+'Type()\n')
    f.write('{\n')
    f.write('\t'+nameAgent+'Package package;\n')
    f.write('\n')
    # lengths and types
    f.write('\tint blockLengths['+str(numAttributes)+'];\n')
    f.write('\tMPI_Datatype typeList['+str(numAttributes)+'];\n')
    f.write('\t// block lengths and types\n')
    # id, position & exists
    f.write('\t// _id\n')
    f.write('\tblockLengths[0] = 32;\n');
    f.write('\ttypeList[0] = MPI_CHAR;\n');
    f.write('\t// _position._x\n')
    f.write('\tblockLengths[1] = 1;\n')
    f.write('\ttypeList[1] = MPI_INT;\n')
    f.write('\t// _position._y\n')
    f.write('\tblockLengths[2] = 1;\n')
    f.write('\ttypeList[2] = MPI_INT;\n')
    f.write('\t// _exists\n')
    f.write('\tblockLengths[3] = 1;\n')
    f.write('\ttypeList[3] = MPI_INT;\n')

    # dynamic params
    index = 4
    for nameAttribute, typeAttribute in attributesMap.items():
        f.write('\t// '+nameAttribute+'\n')
        if typeAttribute == "string":
            f.write('\tblockLengths['+str(index)+'] = 32;\n')
            f.write('\ttypeList['+str(index)+'] = MPI_CHAR;\n');
        else:
            f.write('\tblockLengths['+str(index)+'] = 1;\n')
            mpiTypeAttribute = getMpiTypeAttribute(typeAttribute)
            f.write('\ttypeList['+str(index)+'] = '+mpiTypeAttribute+';\n')
        index = index+1
    f.write('\n')
    # displacements
    f.write('\tMPI_Aint displacements['+str(numAttributes)+'];\n')
    f.write('\tMPI_Aint startAddress;\n')
    f.write('\tMPI_Aint address;\n')
    
    # id, position & exists
    f.write('\t// id\n')
    f.write('\tMPI_Address(package._idMpi, &startAddress);\n')
    f.write('\tdisplacements[0] = 0;\n')

    f.write('\t// _position._x\n')
    f.write('\tMPI_Address(&package._positionMpi._x, &address);\n')
    f.write('\tdisplacements[1] = address-startAddress;\n')

    f.write('\t// _position._y\n')
    f.write('\tMPI_Address(&package._positionMpi._y, &address);\n')
    f.write('\tdisplacements[2] = address-startAddress;\n')

    f.write('\t// _exists\n')
    f.write('\tMPI_Address(&package._existsMpi, &address);\n')
    f.write('\tdisplacements[3] = address-startAddress;\n')

    # dynamic params
    index = 4
    for nameAttribute, typeAttribute in attributesMap.items():
        f.write('\t// '+nameAttribute+'\n')
        f.write('\tMPI_Address(&package.'+nameAttribute+'Mpi, &address);\n')
        f.write('\tdisplacements['+str(index)+'] = address-startAddress;\n')
        index = index+1
    
    # create mpi data type  
    f.write('\n')
    f.write('\tMPI_Datatype * newDataType = new MPI_Datatype;\n')
    f.write('\tMPI_Type_struct('+str(numAttributes)+', blockLengths, displacements, typeList, newDataType);\n')
    f.write('\tMPI_Type_commit(newDataType);\n')
    f.write('\treturn newDataType;\n')
    
    f.write('}\n')
    f.write('\n')
    return None

def createFactoryMethods( listAgents, factoryFile, namespaces, listAttributesMaps ):
    f = open(factoryFile, 'w')
    print '\tcreating mpi factory: ' + factoryFile
    # headers
    f.write('\n')
    f.write('#include <MpiFactory.hxx>\n')
    f.write('#include <Exception.hxx>\n')
    f.write('#include <sstream>\n')
    f.write('\n')
    for i in range(0, len(listAgents)):
        print '\t\tadding: ' + listAgents[i] + ' to factory file: ' + factoryFile
        f.write('#include <'+listAgents[i]+'.hxx>\n')
        f.write('#include "'+listAgents[i]+'_mpi.hxx"\n')
    f.write('\n')
    f.write('namespace Engine\n')
    f.write('{\n')
    f.write('\n')
    
    for i in range(0, len(listAgents)):
        writeCreateType( f, listAgents[i], listAttributesMaps[i])

    writeRegisterTypes( f, listAgents )
    writeCreateDefaultPackage( f, listAgents )
    writeCreateAndFillAgents( f, listAgents, namespaces )

    # close header & namespace
    f.write('} // namespace Engine\n')  
    f.write('\n')   
    f.close()
    return None

def createMpiHeader( agentName, source, header, attributesMap ):
    print '\t\tcreating mpi header: mpiCode/'+agentName+'_mpi.hxx for agent: ' + agentName + ' from source: ' + source + ' and header: ' + header
    f = open('mpiCode/'+agentName+'_mpi.hxx', 'w')
    # header
    f.write('\n')
    f.write('#ifndef __'+agentName+'_mpi_hxx\n')
    f.write('#define __'+agentName+'_mpi_hxx\n')
    f.write('\n')
    f.write('#include <Point2D.hxx>\n')
    f.write('\n')
    # struct Package
    f.write('typedef struct\n')
    f.write('{\n')
    # basic data: _id, _position & _exists
    f.write('\tchar _idMpi[32];\n')
    f.write('\tbool _existsMpi;\n')
    f.write('\tEngine::Point2D<int> _positionMpi;\n')
    f.write('\n')
    # dynamic params
    for nameAttribute, typeAttribute in attributesMap.items():
        if typeAttribute == "string":
            f.write('\tchar '+nameAttribute+'Mpi[32];\n')
        else:
            f.write('\t'+typeAttribute+' '+nameAttribute+'Mpi;\n')
    # close struct
    f.write('} '+agentName+'Package;\n')
    # close file
    f.write('\n')
    f.write('#endif // __'+agentName+'_mpi_hxx\n')
    f.write('\n')
    f.close()
    return None

def writeFillPackage( f, agentName, attributesMap ):
    f.write('void * '+agentName+'::fillPackage()\n')
    f.write('{\n')
    # basic params: _id, _position & _exists
    f.write('\t'+agentName+'Package * package = new '+agentName+'Package;\n')
    f.write('\tmemcpy(&package->_idMpi, _id.c_str(), std::min((unsigned int)32,(unsigned int)(sizeof(char)*_id.size())));\n')
    f.write('\tpackage->_idMpi[std::min((unsigned int)32,(unsigned int)_id.size())] = \'\\0\';\n')
    f.write('\tpackage->_existsMpi = _exists;\n')
    f.write('\tpackage->_positionMpi = _position;\n')
    f.write('\n')
    # dynamic params
    for nameAttribute, typeAttribute in attributesMap.items():
        if typeAttribute == "string":
            f.write('\tmemcpy(&package->'+nameAttribute+'Mpi, '+nameAttribute+'.c_str(), std::min((unsigned int)32,(unsigned int)(sizeof(char)*'+nameAttribute+'.size())));\n');
            f.write('\tpackage->'+nameAttribute+'Mpi[std::min((unsigned int)32,(unsigned int)'+nameAttribute+'.size())] = \'\\0\';\n')
        else:
            f.write('\tpackage->'+nameAttribute+'Mpi = '+nameAttribute+';\n')
    f.write('\treturn package;\n')
    f.write('}\n')
    f.write('\n')
    return None

def writeConstructor( f, agentName, parent , attributesMap ):
    f.write(agentName+'::'+agentName+'( void * package ) : '+parent+'((('+agentName+'Package*)package)->_idMpi)\n')
    f.write('{\n')
    f.write('\t'+agentName+'Package * particularPackage = ('+agentName+'Package*)package;\n')
    # basic params: _position & _exists
    f.write('\t_exists = particularPackage->_existsMpi;\n')
    f.write('\t_position = particularPackage->_positionMpi;\n')
    f.write('\n')
    # dynamic params
    for nameAttribute in attributesMap.keys():
        f.write('\t'+nameAttribute+' = particularPackage->'+nameAttribute+'Mpi;\n')
    f.write('}\n')
    f.write('\n')
    return None

def getMpiTypeConversion( typeInCpp ):
    if typeInCpp == 'int':
        return 'MPI_INTEGER'
    elif typeInCpp == 'float':
        return 'MPI_FLOAT'
    else:
        'Warning, unknown type: ' + typeInCpp + ' using MPI_INTEGER'
        return 'MPI_INTEGER'

def writeVectorAttributesPassing( f, agentName, vectorAttributesMap ):
    print 'writing vector attributes map'
    f.write('\n')
    f.write('void '+agentName+'::sendVectorAttributes( int target )\n')
    f.write('{\n')
    if len(vectorAttributesMap) > 0:
        f.write('\tint sizeVector = 0;\n')
    for nameAttribute in vectorAttributesMap.keys():
        print 'sending vector: ' + nameAttribute + ' with type: ' + vectorAttributesMap[nameAttribute]
        f.write('\tsizeVector = '+nameAttribute+'.size();\n')
        f.write('\tMPI_Send(&sizeVector, 1, MPI_INTEGER, target, Engine::eSizeVector, MPI_COMM_WORLD);\n')
        mpiType = getMpiTypeConversion(vectorAttributesMap[nameAttribute])
        f.write('\tMPI_Send(&'+nameAttribute+'[0], sizeVector, '+mpiType+', target, Engine::eVectorAttribute, MPI_COMM_WORLD);\n')
        f.write('\n')
    f.write('}\n')
    f.write('\n')

    f.write('void '+agentName+'::receiveVectorAttributes( int origin )\n')
    f.write('{\n')
    if len(vectorAttributesMap) > 0:
        f.write('\tint sizeVector = 0;\n')
        f.write('\tMPI_Status status;\n')
    for nameAttribute in vectorAttributesMap.keys():        
        print 'receiving vector: ' + nameAttribute + ' with type: ' + vectorAttributesMap[nameAttribute]
        f.write('\tMPI_Recv(&sizeVector, 1, MPI_INTEGER, origin, Engine::eSizeVector, MPI_COMM_WORLD, &status);\n')
        f.write('\t'+nameAttribute+'.resize(sizeVector);\n')
        mpiType = getMpiTypeConversion(vectorAttributesMap[nameAttribute])
        f.write('\tMPI_Recv(&'+nameAttribute+'[0], sizeVector, '+mpiType+', origin, Engine::eVectorAttribute, MPI_COMM_WORLD, &status);\n')
        f.write('\n')
    f.write('}\n')
    f.write('\n')

def createMpiCode( agentName, source, header, namespace, parent, attributesMap, vectorAttributesMap ):
    print '\t\tcreating mpi file: mpiCode/'+agentName+'_mpi.cxx for agent: ' + agentName + ' in namespace: ' + namespace + ' with parent: ' + parent + ' from source: ' + source + ' and header: ' + header
    f = open('mpiCode/'+agentName+'_mpi.cxx', 'w')
    # header
    f.write('\n')
    f.write('#include "'+agentName+'_mpi.hxx"\n')
    f.write('#include <'+agentName+'.hxx>\n')
    f.write('#include <cstring>\n')
    f.write('#include <mpi.h>\n')
    f.write('#include <typedefs.hxx>\n')
    f.write('\n')
    if namespace!="":
        f.write('namespace '+namespace+'\n')
        f.write('{\n')
        f.write('\n')
    writeFillPackage( f, agentName, attributesMap )
    writeConstructor( f, agentName, parent, attributesMap )
    writeVectorAttributesPassing( f, agentName, vectorAttributesMap );
    if namespace!="":
        f.write('} // namespace '+namespace+'\n')
    f.write('\n')
    f.close()
    return None

# attributes defined as a std::vector of basic types
def addVectorAttribute( line, vectorAttributesMap ):
    indexTemplateBegin = line.find('<')
    indexTemplateEnd= line.find('>')
    typeVector = line[indexTemplateBegin+1:indexTemplateEnd]
    restOfLine = line[indexTemplateEnd+1:]
    indexEndOfName = restOfLine.find(';')
    nameAttribute = restOfLine[:indexEndOfName].strip()
    vectorAttributesMap[nameAttribute] = typeVector
    print '\t\t\tvector attribute detected: ' + nameAttribute + ' with type: std::vector of: ' + typeVector
    return None

# attributes with basic types (int, float, char, ...)
def addBasicAttribute( line, attributesMap ):
    splitLine = line.split()
    # 1st word will be the type of the attribute
    typeAttribute = splitLine[0]
    # 2nd word will be the name, removing final ';'
    nameAttribute = splitLine[1]
    nameAttribute = nameAttribute.strip(';')
    attributesMap[nameAttribute] = typeAttribute
    print '\t\t\tattribute detected: ' + nameAttribute + ' with type: ' + typeAttribute
    return None

def addStringAttribute( line, attributesMap ):
    splitLine = line.split()
    # 1st word will be std::string
    typeAttribute = splitLine[0]
    # 2nd word will be the name, removing final ';'
    nameAttribute = splitLine[1]
    nameAttribute = nameAttribute.strip(';')
    attributesMap[nameAttribute] = 'string'
    print '\t\t\tattribute detected: ' + nameAttribute + ' with type: string'
    return None

def getAttributesFromClass( className, attributesMap, vectorAttributesMap):
    headerName = className+'.hxx'
    print '\t\tlooking for attributes of class: ' +  className + ' in header: '+ headerName + '...'
    f = open(headerName, 'r')
    keyBasic = 'MpiBasicAttribute'  
    keyVector = 'MpiVectorAttribute'
    keyString = 'MpiStringAttribute'
    for line in f:
        if line.find(keyBasic) != -1:
            addBasicAttribute( line, attributesMap )
        elif line.find(keyVector) != -1:
            addVectorAttribute( line, vectorAttributesMap ) 
        elif line.find(keyString) != -1:
            addStringAttribute( line, attributesMap )
        # parse base class, it must inherit from Agent
        elif line.find('class') != -1 and line.find(className) != -1 and line.find('public') != -1:
            splittedLine = line.rsplit()
            parentName = splittedLine[len(splittedLine)-1]
            # remove namespace in case it exists
            parentNameWithoutNamespace = parentName
            indexSeparator = parentName.find('::')
            if(indexSeparator!=-1):
                parentNameWithoutNamespace = parentName[indexSeparator+2:]
            if parentNameWithoutNamespace!= 'Agent':
                getAttributesFromClass( parentNameWithoutNamespace, attributesMap, vectorAttributesMap )
    f.close()
    return parentName

def checkHeader(agentName, headerName):
    print '\tchecking if header: ' + headerName + ' for agent: ' + agentName + ' defines needed methods...'
    # if this is not defined, we will add the four needed methods
    fillPackageName = 'fillPackage'
    f = open(headerName, 'r')
    for line in f:
        if line.find(fillPackageName) != -1:
            print '\theader: ' + headerName + ' correct'
            return
    f.close()
    print '\theader: ' + headerName + ' does not contain parallel methods, adding...'

    headerNameTmp = headerName + '_tmp'
    f = open(headerName, 'r')
    fTmp = open(headerNameTmp, 'w')

    insideClass = 0
    lastLine = ''
    for line in f:
        if insideClass == 0:
            if line.find('class')!=-1 and line.find(agentName) != -1:
                print 'accessing agent declaration: ' + agentName
                insideClass = 1
            fTmp.write(line)
        elif line.find('};') != -1:
            print 'end of agent declaration: ' + agentName
            insideClass = 0
            fTmp.write('\n')
            fTmp.write('\t////////////////////////////////////////////////\n')
            fTmp.write('\t// This code has been automatically generated //\n')
            fTmp.write('\t/////// Please do not modify it ////////////////\n')
            fTmp.write('\t////////////////////////////////////////////////\n')
            fTmp.write('\t'+agentName+'( void * );\n')
            fTmp.write('\tvoid * fillPackage();\n')
            fTmp.write('\tvoid sendVectorAttributes(int);\n')
            fTmp.write('\tvoid receiveVectorAttributes(int);\n')
            fTmp.write('\t////////////////////////////////////////////////\n')
            fTmp.write('\t//////// End of generated code /////////////////\n')
            fTmp.write('\t////////////////////////////////////////////////\n')
            fTmp.write('\n')
            fTmp.write(line)
        else:
            fTmp.write(line)
    f.close()
    fTmp.close()
    os.rename(headerNameTmp, headerName)
    return 

def execute( target, source, env ):
    print 'generating code for mpi communication...'
    listAgents = []
    listAttributesMaps = []
    namespaceAgents = env['namespaces']
    for i in range(1,len(source)):      
        sourceName = str(source[i])
        headerName = sourceName.replace(".cxx", ".hxx")
        listAgents += [sourceName.replace(".cxx", "")]
        checkHeader(sourceName.replace(".cxx", ""), headerName)
        print '\tprocessing agent: ' + listAgents[i-1]
        # get the list of attributes to send/receive in MPI
        attributesMap = {}
        vectorAttributesMap = {}
        parentName = getAttributesFromClass(listAgents[i-1], attributesMap, vectorAttributesMap )
        # create header declaring a package with the list of attributes
        createMpiHeader(listAgents[i-1], sourceName, headerName, attributesMap )
        # create a source code defining package-class copy
        createMpiCode(listAgents[i-1], sourceName, headerName, namespaceAgents[i-1], parentName, attributesMap, vectorAttributesMap )
        listAttributesMaps.append(attributesMap)

    # fill mpi code registering types and additional methods
    createFactoryMethods(listAgents, str(target[0]), namespaceAgents, listAttributesMaps )
    print 'done!'
    return None

