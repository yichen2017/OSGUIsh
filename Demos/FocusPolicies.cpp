/******************************************************************************\
* FocusPolicies.cpp                                                            *
* Showcase for the various focus policies for keyboard and mouse wheel events. *
* Leandro Motta Barros                                                         *
\******************************************************************************/

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgProducer/Viewer>
#include <osgText/Text>
#include <OSGUIsh/EventHandler.hpp>
#include <OSGUIsh/MouseOverFocusPolicy.hpp>
#include <OSGUIsh/MouseDownFocusPolicy.hpp>

//
// Some globals (globals are not a problem in simple examples ;-))
//

OSGUIsh::NodePtr NodeUnderMouse;
osg::ref_ptr<osg::Node> TreeNode;
osg::ref_ptr<osg::Node> StrawberryNode;
osg::ref_ptr<osg::Node> FishNode;
osg::ref_ptr<osgText::Text> TextMouseOver;
osg::ref_ptr<osgText::Text> TextMouseWheelEvent;
osg::ref_ptr<osgText::Text> TextKeyboardEvent;
osg::ref_ptr<osgText::Text> TextMouseWheelFocusPolicy;
osg::ref_ptr<osgText::Text> TextKeyboardFocusPolicy;

//
// The event handlers
//

void HandleMouseEnter (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   TextMouseOver->setText ("Mouse over " + node->getName());
}

void HandleMouseLeave (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   TextMouseOver->setText ("Mouse over nothing!");
}

void HandleMouseWheelUp (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   TextMouseWheelEvent->setText (node->getName() + ": Mouse wheel up!");
}

void HandleMouseWheelDown (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   TextMouseWheelEvent->setText (node->getName() + ": Mouse wheel down!");
}

void HandleKeyDown (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   TextKeyboardEvent->setText (node->getName() + ": key down: "
                               + static_cast<char>(ea.getKey()));
}

void HandleKeyUp (const osgGA::GUIEventAdapter& ea, OSGUIsh::NodePtr node)
{
   TextKeyboardEvent->setText (node->getName() + ": key up: "
                               + static_cast<char>(ea.getKey()));
}



// - CreateHUD -----------------------------------------------------------------
osg::ref_ptr<osg::Projection> CreateHUD (int width, int height)
{
   // Create the text nodes to be displayed on the HUD
   osg::ref_ptr<osg::Geode> hudGeometry (new osg::Geode());

   TextMouseOver = new osgText::Text;
   TextMouseOver->setText ("Mouse over nothing!");
   TextMouseOver->setFont ("Data/bluehigl.ttf");
   TextMouseOver->setPosition (osg::Vec3 (10.0f, 10.0f, 0.0f));
   TextMouseOver->setCharacterSize (25.0);
   hudGeometry->addDrawable (TextMouseOver.get());

   TextMouseWheelEvent = new osgText::Text;
   TextMouseWheelEvent->setText ("Waiting for mouse wheel event");
   TextMouseWheelEvent->setFont ("Data/bluehigl.ttf");
   TextMouseWheelEvent->setPosition (osg::Vec3 (10.0f, 40.0f, 0.0f));
   TextMouseWheelEvent->setCharacterSize (25.0);
   hudGeometry->addDrawable (TextMouseWheelEvent.get());

   TextKeyboardEvent = new osgText::Text;
   TextKeyboardEvent->setText ("Waiting for keyboard event");
   TextKeyboardEvent->setFont ("Data/bluehigl.ttf");
   TextKeyboardEvent->setPosition (osg::Vec3 (10.0f, 70.0f, 0.0f));
   TextKeyboardEvent->setCharacterSize (25.0);
   hudGeometry->addDrawable (TextKeyboardEvent.get());

   TextMouseWheelFocusPolicy = new osgText::Text;
   TextMouseWheelFocusPolicy->setText ("Mouse wheel focus policy");
   TextMouseWheelFocusPolicy->setFont ("Data/bluehigl.ttf");
   TextMouseWheelFocusPolicy->setPosition (osg::Vec3 (10.0f, 100.0f, 0.0f));
   TextMouseWheelFocusPolicy->setCharacterSize (25.0);
   hudGeometry->addDrawable (TextMouseWheelFocusPolicy.get());

   TextKeyboardFocusPolicy = new osgText::Text;
   TextKeyboardFocusPolicy->setText ("Keyboard focus policy");
   TextKeyboardFocusPolicy->setFont ("Data/bluehigl.ttf");
   TextKeyboardFocusPolicy->setPosition (osg::Vec3 (10.0f, 130.0f, 0.0f));
   TextKeyboardFocusPolicy->setCharacterSize (25.0);
   hudGeometry->addDrawable (TextKeyboardFocusPolicy.get());

   // Create the HUD per se
   osg::ref_ptr<osg::StateSet> stateSet = hudGeometry->getOrCreateStateSet();
   stateSet->setMode (GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode (GL_DEPTH_TEST, osg::StateAttribute::OFF);
   stateSet->setRenderBinDetails (11, "RenderBin");

   osg::ref_ptr<osg::MatrixTransform> modelviewAbs (new osg::MatrixTransform);
   modelviewAbs->setReferenceFrame (osg::Transform::ABSOLUTE_RF);
   modelviewAbs->setMatrix (osg::Matrix::identity());

   modelviewAbs->addChild (hudGeometry.get());

   osg::ref_ptr<osg::Projection> projection (new osg::Projection());
   projection->setMatrix (osg::Matrix::ortho2D (0, width, 0, height));
   projection->addChild (modelviewAbs.get());

   return projection;
}



// - LoadModel -----------------------------------------------------------------
osg::ref_ptr<osg::Node> LoadModel (const std::string& fileName)
{
   osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile (fileName);

   if (!loadedModel)
   {
      std::cerr << "Problem opening '" << fileName << "'\n";
      exit (1);
   }

   return loadedModel;
}



// - LoadModels ----------------------------------------------------------------
osg::ref_ptr<osg::Group> LoadModels()
{
   osg::ref_ptr<osg::Group> group (new osg::Group);

   TreeNode = LoadModel ("Data/Tree_01.3ds");
   TreeNode->setName ("Tree");
   osg::ref_ptr<osg::PositionAttitudeTransform> treePAT(
      new osg::PositionAttitudeTransform());
   treePAT->addChild (TreeNode.get());
   treePAT->setPosition (osg::Vec3 (1.2, 0.0, 0.0));
   group->addChild (treePAT.get());

   StrawberryNode = LoadModel ("Data/Strawberry.3ds");
   StrawberryNode->setName ("Strawberry");
   osg::ref_ptr<osg::PositionAttitudeTransform> strawberryPAT(
      new osg::PositionAttitudeTransform());
   strawberryPAT->addChild (StrawberryNode.get());
   strawberryPAT->setPosition (osg::Vec3 (-1.0, 0.0, 0.0));
   group->addChild (strawberryPAT.get());

   FishNode = LoadModel ("Data/Fish.3ds");
   FishNode->setName ("Fish");

   group->addChild (FishNode.get());

   return group;
}



// - main ----------------------------------------------------------------------
int main (int argc, char* argv[])
{
   // Create a Producer-based viewer
   osgProducer::Viewer viewer;
   viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

   osg::ref_ptr<OSGUIsh::EventHandler> guishEH(
      new OSGUIsh::EventHandler(
         viewer,
         OSGUIsh::FocusPolicyFactoryMason<OSGUIsh::MouseOverFocusPolicy>(),
         OSGUIsh::FocusPolicyFactoryMason<OSGUIsh::MouseDownFocusPolicy>()));

   viewer.getEventHandlerList().push_front (guishEH.get());

   // Construct the scene graph, set it as the data to be viewed
   osg::ref_ptr<osg::Group> sgRoot = LoadModels().get();
   sgRoot->addChild (CreateHUD (1024, 768).get());
   viewer.setSceneData (sgRoot.get());

   // Adds the node to the event handler, so that it can get events
   guishEH->addNode (TreeNode);
   guishEH->addNode (StrawberryNode);
   guishEH->addNode (FishNode);

   // Register event handlers
   guishEH->getSignal (TreeNode, "MouseEnter")->connect (&HandleMouseEnter);
   guishEH->getSignal (StrawberryNode, "MouseEnter")->connect (&HandleMouseEnter);
   guishEH->getSignal (FishNode, "MouseEnter")->connect (&HandleMouseEnter);

   guishEH->getSignal (TreeNode, "MouseLeave")->connect (&HandleMouseLeave);
   guishEH->getSignal (StrawberryNode, "MouseLeave")->connect (&HandleMouseLeave);
   guishEH->getSignal (FishNode, "MouseLeave")->connect (&HandleMouseLeave);

   guishEH->getSignal (TreeNode, "MouseWheelUp")->connect (&HandleMouseWheelUp);
   guishEH->getSignal (StrawberryNode, "MouseWheelUp")->connect (&HandleMouseWheelUp);
   guishEH->getSignal (FishNode, "MouseWheelUp")->connect (&HandleMouseWheelUp);

   guishEH->getSignal (TreeNode, "MouseWheelDown")->connect (&HandleMouseWheelDown);
   guishEH->getSignal (StrawberryNode, "MouseWheelDown")->connect (&HandleMouseWheelDown);
   guishEH->getSignal (FishNode, "MouseWheelDown")->connect (&HandleMouseWheelDown);

   guishEH->getSignal (TreeNode, "KeyDown")->connect (&HandleKeyDown);
   guishEH->getSignal (StrawberryNode, "KeyDown")->connect (&HandleKeyDown);
   guishEH->getSignal (FishNode, "KeyDown")->connect (&HandleKeyDown);

   // An OpenGL window, please
   viewer.realize();

   // Enter rendering loop
   while (!viewer.done())
   {
      viewer.sync();
      viewer.update();
      viewer.frame();
   }

   viewer.sync();
}