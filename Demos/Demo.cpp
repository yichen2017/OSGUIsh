/******************************************************************************\
* Demo.cpp                                                                     *
* OSGUIsh demo.                                                                *
* Leandro Motta Barros                                                         *
\******************************************************************************/

#include <osgDB/ReadFile>
#include <osgProducer/Viewer>
#include <OSGUIsh/EventHandler.hpp>


void HandleMouseMove (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Mouse moved on node '" << node.get() << "'!\n";
}

void HandleMouseEnter (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Entered node '" << node.get() << "'!\n";
}

void HandleMouseLeave (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Left node '" << node.get() << "'!\n";
}

void HandleMouseDown (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Mouse down on node '" << node.get() << "'!\n";
}

void HandleMouseUp (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Mouse up on node '" << node.get() << "'!\n";
}

void HandleClick (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Click on node '" << node.get() << "'!\n";
}

void HandleDoubleClick (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Double click on node '" << node.get() << "'!\n";
}

void HandleKeyDown (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Key down on node '" << node.get() << "'!\n";
}

void HandleKeyUp (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Key up on node '" << node.get() << "'!\n";
}

void HandleMouseWheelUp (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Mouse wheel up on node '" << node.get() << "'!\n";
}

void HandleMouseWheelDown (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   std::cout << "Mouse wheel down on node '" << node.get() << "'!\n";
}



// - main ----------------------------------------------------------------------
int main (int argc, char* argv[])
{
   // Create a Producer-based viewer
   osgProducer::Viewer viewer;
   viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

   osg::ref_ptr<OSGUIsh::EventHandler> guishEH(
      new OSGUIsh::EventHandler (viewer));

   viewer.getEventHandlerList().push_front (guishEH.get());

   // Load the model
   osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile ("Data/Tree_01.3ds");

   if (!loadedModel)
   {
      std::cerr << "Problem opening 'Data/Tree_01.3ds'\n";
      exit (1);
   }

   viewer.setSceneData (loadedModel.get());

   // Register event handlers
   guishEH->addNode (loadedModel);

   guishEH->getSignal (loadedModel, "MouseMove")->connect (&HandleMouseMove);
   guishEH->getSignal (loadedModel, "MouseEnter")->connect (&HandleMouseEnter);
   guishEH->getSignal (loadedModel, "MouseLeave")->connect (&HandleMouseLeave);
   guishEH->getSignal (loadedModel, "MouseDown")->connect (&HandleMouseDown);
   guishEH->getSignal (loadedModel, "MouseUp")->connect (&HandleMouseUp);
   guishEH->getSignal (loadedModel, "Click")->connect (&HandleClick);
   guishEH->getSignal (loadedModel, "DoubleClick")->connect (&HandleDoubleClick);

   guishEH->setKeyboardFocus (loadedModel);
   guishEH->getSignal (loadedModel, "KeyDown")->connect (&HandleKeyDown);
   guishEH->getSignal (loadedModel, "KeyUp")->connect (&HandleKeyUp);

   guishEH->setMouseWheelFocus (loadedModel);
   guishEH->getSignal (loadedModel, "MouseWheelUp")->connect (&HandleMouseWheelUp);
   guishEH->getSignal (loadedModel, "MouseWheelDown")->connect (&HandleMouseWheelDown);

   // Enter rendering loop
   viewer.realize();

   while (!viewer.done())
   {
      viewer.sync();
      viewer.update();
      viewer.frame();
   }

   viewer.sync();
}
