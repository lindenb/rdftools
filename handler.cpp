
#include "handler.hh"

using namespace std;

RDFHandler::RDFHandler()
	{
	}


int
RDFHandler::visit(const RDFEvent* evt)
	{
	return 0;
	}

std::auto_ptr<std::string> RDFHandler::getAttribute(xmlTextReaderPtr r,const xmlChar * localName,const xmlChar * namespaceURI)
	{
	std::auto_ptr<std::string> s;
	xmlChar * att= ::xmlTextReaderGetAttributeNs(r,localName,namespaceURI);
	if(att!=NULL)
		{
		s.reset(new string((const char*)att));
		::xmlFree(att);
		}
	return s;
	}

std::auto_ptr<std::string> RDFHandler::getAttribute(xmlTextReaderPtr r,const xmlChar * name)
	{
	std::auto_ptr<std::string> s;
	xmlChar * att= ::xmlTextReaderGetAttribute(r,name);
	if(att!=NULL)
		{
		s.reset(new string((const char*)att));
		::xmlFree(att);
		}
	return s;
	}	

void RDFHandler::parse(xmlTextReaderPtr r)
	{
	int status;
	/** loop until we find a rdf:RDF element */
	while((status=::xmlTextReaderRead(r))!=0)
		{
		if(status<0) return;//BOUMMMMM TOD
		if( ::xmlTextReaderNodeType(r) != XmlNodeType.Element ) continue;
		const xmlChar *ns=xmlTextReaderConstNamespaceUri(r);
		if(ns==NULL || strcmp(ns,RDF.NS)!=0) continue;
		const xmlChar *localName = ::xmlTextReaderConstLocalName(r);
		if(localName==NULL || strcmp(ns,"RDF")!=0) continue;
		this->parseRDF(r);
		}
	}
	
void RDFHandler::parseRDF(xmlTextReaderPtr r) throws XMLStreamException,URISyntaxException
	{
	int status;
	DEBUG("In RDF ROOT: loop over each rdf:Description");
	while((status=::xmlTextReaderRead(r))!=0)
		{
		if(status<0) return;//BOUMMMMM TOD
		
		switch( XmlNodeType.Element  )
			{
			case(XmlNodeType.EndElement)
				{
				return;
				}
			case(XmlNodeType.Element)
				{
				parseDescription(r,event.asStartElement());
				break;
				}
			case(XmlNodeType.ProcessingInstruction)
				{
				//throw new XMLStreamException("Found Processing Instruction in RDF ???");
				break;
				}
			case(XmlNodeType.Text)
				{
				//throw new XMLStreamException("Found text in RDF ???");
				}
			}
		}
	}


Resource RDFHandler::parseDescription(xmlTextReaderPtr r)
	{
	debug("Found a new  rdf:Description "+description.getName());
	Resource description(
		::xmlTextReaderConstNamespaceUri(r),
		::xmlTextReaderConstLocalName(r)
		);
	Resource descriptionURI=null;
	auto_ptr<string> att= this->getAttribute("about",RDF.NS);
	if(att.get()!=NULL) descriptionURI=  this->createURI(att.get()->c_str());
	if(descriptionURI==null)
		{
		att= this->getAttribute("nodeID",RDF.NS);
		if(att.get()!=null) descriptionURI=   this->createURI(att.get()->c_str());
		}
	if(descriptionURI==null)
		{
		att= this->getAttribute("ID",RDF.NS);
		if(att.get()!=null) descriptionURI= this->resolveBase(att.get()->c_str());
		}
	
	if(descriptionURI==null)
		{
		descriptionURI= this->createAnonymousURI();
		}
	
	DEBUG("Description uri=\""+descriptionURI+"\"");
	
	QName qn=description.getName();
	if(!(description.equals(RDF.NS,"Description")))
		{
		RDFEvent evt;
		evt.subject=descriptionURI;
		evt.predicate= createURI(RDF.NS+"type");
		evt.value=name2uri(qn);
		found(evt);
		}

	/** loop over attributes */
	if(xmlTextReaderHasAttributes(r))
		{
		if(!xmlTextReaderMoveToFirstAttribute(r)) throw boum;
		do
			{
			
			att=(Attribute)i.next();
			qn= att.getName();
			String local= qn.getLocalPart();
			if(qn.getNamespaceURI().equals(RDF.NS) &&
				( local.equals("about") ||
					local.equals("ID") ||
					local.equals("nodeID")))
					{
					continue;
					}

			RDFEvent evt;
			evt.subject=descriptionURI;
			evt.predicate= name2uri(qn);
			evt.value= att.getValue();
			visit(&evt);
			} while(xmlTextReaderMoveToNextAttribute(r));
		}
	

	
	while((status=::xmlTextReaderRead(r))!=0)
		{
		if(status<0) return;//BOUMMMMM TOD
		
		switch( XmlNodeType.Element  )
			{
			case(XmlNodeType.EndElement)
				{
				return description;
				}
			case(XmlNodeType.Element)
				{
				parsePredicate(r,description);
				break;
				}
				
			}
		
		else if(event.isProcessingInstruction())
			{
			throw new XMLStreamException("Found Processing Instruction in RDF ???");
			}
		else if(event.isCharacters() &&
				event.asCharacters().getData().trim().length()>0)
			{
			throw new XMLStreamException("Found text in RDF ??? \""+
				event.asCharacters().getData()+"\""	
				);
			}
		}

	return description;
	}



/**
 * parse predicate
 * @param descriptionURI
 * @param predicate 
 * @throws URISyntaxException
 * @throws XMLStreamException
 */
private void parsePredicate(URI descriptionURI,StartElement predicate) throws URISyntaxException,XMLStreamException
	{
	String parseType=null;
	String lang=null;
	URI datatype=null;
	Attribute att;
	QName qn=null;
	URI resource=null;

	URI predicateURI=name2uri(predicate.getName());
	debug("parse rdf:description=\""+descriptionURI+"\" predicate:"+predicateURI);
	
	/** collect attributes */
	for(int loop=0;loop<2;++loop)
		{
		for(Iterator<?> i=predicate.getAttributes();
			i.hasNext();)
			{
			att=(Attribute)i.next();
			qn= att.getName();
			String local= qn.getLocalPart();
			if(qn.getPrefix().equals("xml") &&
				local.equals("lang"))
				{
				if(loop==0) lang=att.getValue();
				continue;
				}
			else if(qn.getNamespaceURI().equals(RDF.NS))
				{
				if(local.equals("parseType"))
					{
					if(loop==0)  parseType=att.getValue();
					debug("parseType:"+parseType);
					continue;
					}
				else if(local.equals("datatype"))
					{
					if(loop==0) datatype= createURI(att.getValue());
					debug("dataType="+datatype);
					continue;
					}
				else if(local.equals("resource"))
					{
					if(loop==0) resource=  createURI(att.getValue());
					debug("rdf:resource="+resource);
					continue;
					}
				else if(local.equals("nodeID"))
					{
					if(loop==0) resource=  createURI(att.getValue());
					debug("rdf:nodeID="+resource);
					continue;
					}
				else if(local.equals("ID"))
					{
					if(loop==0) resource= resolveBase(att.getValue());
					debug("ID="+resource);
					continue;
					}
				}
			
			if(loop==1)
				{
				if(resource!=null)
					{
					debug(resource);
					RDFEvent evt= new RDFEvent();
					evt.subject= resource;
					evt.predicate= name2uri(att.getName());
					evt.value=att.getValue();
					found(evt);
					}
				else
					{
					throw new XMLStreamException("Cannot handle attribute "+att);
					}
				}
			
			}
		}
	
	
	
	
	if(resource!=null)
		{
		RDFEvent evt= new RDFEvent();
		evt.subject=descriptionURI;
		evt.predicate= predicateURI;
		evt.value=resource;
		found(evt);
		debug();
		XMLEvent event=getReader().peek();
		if(event!=null && event.isEndElement())
			{
			debug();
			getReader().nextEvent();
			return;
			}
		
		throw new XMLStreamException("Expected a EndElement for this element");
		}
	
	if(parseType==null) parseType="default";
	
	if(parseType.equals("Literal"))
		{
		StringBuilder b= parseLiteral();
		
		RDFEvent evt= new RDFEvent();
		evt.subject=descriptionURI;
		evt.predicate= predicateURI;
		evt.value= b.toString();
		evt.lang=lang;
		evt.valueType=datatype;
		found(evt);
		}
	else if(parseType.equals("Resource"))
		{
		URI blanck = createAnonymousURI();
		
		RDFEvent evt= new RDFEvent();
		evt.subject=descriptionURI;
		evt.predicate= predicateURI;
		evt.value=blanck;
		evt.lang=lang;
		evt.valueType=datatype;
		
		
		
		found(evt);
		while(getReader().hasNext())
			{
			XMLEvent event = getReader().nextEvent();
			if(event.isStartElement())
				{
				parsePredicate(blanck, event.asStartElement());
				}
			else if(event.isEndElement())
				{
				return;
				}
			}
		
		}
	else  if(parseType.equals("Collection"))
		{
		int index=0;
		while(getReader().hasNext())
			{
			XMLEvent event = getReader().nextEvent();
			if(event.isStartElement())
				{
				URI value= parseDescription(event.asStartElement());
				
				RDFEvent evt= new RDFEvent();
				evt.subject=descriptionURI;
				evt.predicate= predicateURI;
				evt.value=value;
				evt.lang=lang;
				evt.valueType=datatype;
				evt.listIndex=(++index);
				
				found(evt);
				}
			else if(event.isEndElement())
				{
				return;
				}
			}
		}
	else
		{
		boolean foundResourceAsChild=false;
		StringBuilder b= new StringBuilder();
		while(getReader().hasNext())
			{
			XMLEvent event = getReader().nextEvent();
			if(event.isStartElement())
				{
				if(b.toString().trim().length()!=0)
					{
					throw new XMLStreamException(
							"Bad text \""+b+"\" before "+
							event.asStartElement().getName()
							);
					}
				URI childURI=parseDescription(event.asStartElement());
				RDFEvent evt= new RDFEvent();
				evt.subject=descriptionURI;
				evt.predicate= predicateURI;
				evt.value= childURI;
				found(evt);
				b.setLength(0);
				foundResourceAsChild=true;
				}
			else if(event.isCharacters())
				{
				b.append(event.asCharacters().getData());
				}
			else if(event.isEndElement())
				{
				if(!foundResourceAsChild)
					{
					RDFEvent evt= new RDFEvent();
					evt.subject=descriptionURI;
					evt.predicate= predicateURI;
					evt.value= b.toString();
					evt.lang=lang;
					evt.valueType=datatype;
					found(evt);
					}
				else
					{
					if(b.toString().trim().length()!=0) throw new XMLStreamException("Found bad text "+b);
					}
				return;
				}
			}
		}
	
}

private URI resolveBase(String ID) throws URISyntaxException
	{
	if(this.base==null) return  createURI(ID);
	return this.base.resolve(ID);
	}

private StringBuilder parseLiteral() throws XMLStreamException
	{
	StringBuilder b=new StringBuilder();
	QName qn;
	int depth=0;
	while(getReader().hasNext())
		{
		XMLEvent event = getReader().nextEvent();
		if(event.isCharacters())
			{
			b.append(escapeXML(event.asCharacters().getData()));
			}
		else if(event.isProcessingInstruction())
			{
			b.append("<?"+event.asCharacters()+"?>");
			}
		else if(event.isEndElement())
			{
			if(depth==0) return b;
			qn= event.asEndElement().getName();
			b.append("</"+qn.getPrefix()+":"+qn.getLocalPart()+">");
			depth--;
			}
		else if(event.isStartElement())
			{
			qn= event.asEndElement().getName();
			b.append("<"+qn.getPrefix()+":"+qn.getLocalPart());
			
			for(Iterator<?> i=event.asStartElement().getAttributes();
				i.hasNext();)
				{
				Attribute att=(Attribute)i.next();
				qn= att.getName();
				b.append(" ").
					append(qn.getPrefix()+":"+qn.getLocalPart()).
					append("=\"").
					append(escapeXML(att.getValue())).
					append("\"");
				}
			event=getReader().peek();
			if(event!=null && event.isEndElement())
				{
				getReader().nextEvent();
				b.append("/>");
				}
			else
				{
				b.append(">");
				depth++;
				}
			}
		}
	
	return b;
	}

protected URI createAnonymousURI() throws URISyntaxException
	{
	return createURI("_"+(++ID_GENERATOR));
	}

public void parse(InputStream in) throws XMLStreamException
	{
	read(new InputStreamReader(in));
	}

public void parse(Reader in) throws XMLStreamException
	{
	read(in);
	}

public void parse(File in) throws XMLStreamException
	{
	try {
		debug("Parsing file "+in);
		FileReader fin= new FileReader(in);
		read(fin);
		fin.close();
		debug("End Parsing file "+in);
		}
	catch (IOException e)
		{
		throw new XMLStreamException(e);
		}
	}

public void parse(URL in) throws XMLStreamException
	{
	debug("parsing URL "+in);
	try {
		InputStream fin= in.openStream();
		read(new InputStreamReader(fin));
		fin.close();
		}
	catch (IOException e)
		{
		throw new XMLStreamException(e);
		}
	}

private String name2string(StartElement e)
	{
	return name2string(e.getName());
	}

private String name2string(QName name)
	{
	return name.getNamespaceURI()+name.getLocalPart();
	}


private  URI name2uri(QName name) throws URISyntaxException 
	{
	return createURI(name2string(name));
	}



private URI createURI(String uriAsString) throws URISyntaxException 
	{
	return  new URI(uriAsString);
	}


private static String escapeXML(String s)
	{
	return XMLUtilities.escape(s);
	}

/**
 * echo URLs
 * @param args
 */
public static void main(String[] args)
	{
	try {
		Debug.setDebugging(false);
    	int optind=0;
    	
    	while(optind<args.length)
		        {
		        if(args[optind].equals("-h"))
		           {
		        	System.err.println(Compilation.getLabel());
		        	System.err.println("\t-h this screen");
		        	System.err.println("\t-d turns debugging on");
					return;
		           	}
		        else if(args[optind].equals("-d"))
		        	{
		        	Debug.setDebugging(true);
		        	}
		       else if(args[optind].equals("--"))
		            {
		            ++optind;
		            break;
		            }
		        else if(args[optind].startsWith("-"))
		            {
		            throw new IllegalArgumentException("Unknown option "+args[optind]);
		            }
		        else
		            {
		            break;    
		            }
		        ++optind;
		        }
    	
		System.out.println("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
		System.out.println("<rdf:RDF  xmlns:rdf=\""+RDF.NS+"\">");
		
		RDFHandler h= new RDFHandler()
			{
			@Override
			public void found(RDFEvent event) {
				System.out.println(event);
				}
			
			};
		
		while(optind< args.length)
			{
			h.parse(new URL(args[optind++]));
			}
		
		System.out.println("</rdf:RDF>");
		
	} catch (Exception e) {
		e.printStackTrace();
	}
	}

}
