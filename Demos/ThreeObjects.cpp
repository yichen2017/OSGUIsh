/******************************************************************************\
* ThreeObjects.cpp                                                             *
* Two objects sharing some event handlers, one that doesn't receive events.    *
* Leandro Motta Barros                                                         *
\******************************************************************************/

#include <iostream>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgText/Text>
#include <OSGUIsh/EventHandler.hpp>

//
// Some globals (globals are not a problem in simple examples ;-))
//

osg::ref_ptr<osg::Node> TreeNode;
osg::ref_ptr<osg::Node> StrawberryNode;
osg::ref_ptr<osg::Node> FishNode;
osg::ref_ptr<osgText::Text> TextMouseOver;
osg::ref_ptr<osgText::Text> TextDoubleClicked;

//
// The event handlers
//

void HandleMouseEnter(OSGUIsh::HandlerParams& params)
{
   TextMouseOver->setText("Mouse over " + params.node->getName());
}

void HandleMouseLeave(OSGUIsh::HandlerParams& params)
{
   TextMouseOver->setText("Mouse over nothing vegetable!");
}

void HandleDoubleClickTree(OSGUIsh::HandlerParams& params)
{
   TextDoubleClicked->setText("Just a tree, not three!");
}

void HandleDoubleClickStrawberry(OSGUIsh::HandlerParams& params)
{
   TextDoubleClicked->setText("A lone, field-less strawberry.");
}



// - CreateHUD -----------------------------------------------------------------
osg::ref_ptr<osg::Projection> CreateHUD (int width, int height)
{
   // Create the text nodes to be displayed on the HUD
   osg::ref_ptr<osg::Geode> hudGeometry (new osg::Geode());

   TextMouseOver = new osgText::Text();
   TextMouseOver->setDataVariance(osg::Object::DYNAMIC);
   TextMouseOver->setText("Mouse over nothing vegetable!");
   TextMouseOver->setFont("Data/bluehigl.ttf");
   TextMouseOver->setPosition(osg::Vec3 (10.0f, 10.0f, 0.0f));
   TextMouseOver->setCharacterSize(25.0);
   hudGeometry->addDrawable(TextMouseOver);

   TextDoubleClicked = new osgText::Text();
   TextDoubleClicked->setDataVariance(osg::Object::DYNAMIC);
   TextDoubleClicked->setText("Try double clicking vegetables!");
   TextDoubleClicked->setFont("Data/bluehigl.ttf");
   TextDoubleClicked->setPosition(osg::Vec3 (10.0f, 40.0f, 0.0f));
   TextDoubleClicked->setCharacterSize(25.0);
   hudGeometry->addDrawable(TextDoubleClicked);

   // Create the HUD per se
   osg::ref_ptr<osg::StateSet> stateSet = hudGeometry->getOrCreateStateSet();
   stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   stateSet->setRenderBinDetails(11, "RenderBin");

   osg::ref_ptr<osg::MatrixTransform> modelviewAbs(new osg::MatrixTransform);
   modelviewAbs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   modelviewAbs->setMatrix(osg::Matrix::identity());

   modelviewAbs->addChild(hudGeometry);

   osg::ref_ptr<osg::Projection> projection(new osg::Projection());
   projection->setMatrix(osg::Matrix::ortho2D(0, width, 0, height));
   projection->addChild(modelviewAbs);

   return projection;
}



// - LoadModel -----------------------------------------------------------------
osg::ref_ptr<osg::Node> LoadModel(const std::string& fileName)
{
   osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(fileName);

   if (!loadedModel)
   {
      std::cerr << "Problem opening '" << fileName << "'\n";
      exit(1);
   }

   return loadedModel;
}



// - LoadModels ----------------------------------------------------------------
osg::ref_ptr<osg::Group> LoadModels()
{
   osg::ref_ptr<osg::Group> group(new osg::Group);

   TreeNode = LoadModel("Data/Tree_01.3ds");
   TreeNode->setName("Tree");
   osg::ref_ptr<osg::PositionAttitudeTransform> treePAT(
      new osg::PositionAttitudeTransform());
   treePAT->addChild(TreeNode);
   treePAT->setPosition(osg::Vec3(1.2, 0.0, 0.0));
   group->addChild(treePAT);

   StrawberryNode = LoadModel("Data/Strawberry.3ds");
   StrawberryNode->setName("Strawberry");
   osg::ref_ptr<osg::PositionAttitudeTransform> strawberryPAT(
      new osg::PositionAttitudeTransform());
   strawberryPAT->addChild(StrawberryNode);
   strawberryPAT->setPosition(osg::Vec3(-1.0, 0.0, 0.0));
   group->addChild(strawberryPAT);

   FishNode = LoadModel("Data/Fish.3ds");
   FishNode->setName("Fish");

   group->addChild(FishNode);

   return group;
}



// - main ----------------------------------------------------------------------
int main(int argc, char* argv[])
{
   // Create a viewer
   osgViewer::Viewer viewer;
   viewer.setUpViewInWindow(0, 0, 1024, 768);

   // Construct the scene graph, set it as the data to be viewed
   osg::ref_ptr<osg::Group> sgRoot = LoadModels();
   sgRoot->addChild(CreateHUD(1024, 768));
   viewer.setSceneData(sgRoot);

   // Create the OSGUIsh event handler. For testing purposes, use a positive
   // picking radius, even though this will not make much difference.
   osg::ref_ptr<OSGUIsh::EventHandler> guishEH(new OSGUIsh::EventHandler(0.01));

   viewer.addEventHandler(guishEH);

   // Adds the node to the event handler, so that it can get events
   guishEH->addNode(TreeNode);
   guishEH->addNode(StrawberryNode);

   // Register event handlers
   guishEH->getSignal(TreeNode, OSGUIsh::EVENT_MOUSE_ENTER)
      ->connect(&HandleMouseEnter);
   guishEH->getSignal(StrawberryNode, OSGUIsh::EVENT_MOUSE_ENTER)
      ->connect(&HandleMouseEnter);
   guishEH->getSignal(TreeNode, OSGUIsh::EVENT_MOUSE_LEAVE)
      ->connect(&HandleMouseLeave);
   guishEH->getSignal(StrawberryNode, OSGUIsh::EVENT_MOUSE_LEAVE)
      ->connect(&HandleMouseLeave);
   guishEH->getSignal(StrawberryNode, OSGUIsh::EVENT_DOUBLE_CLICK)
      ->connect(&HandleDoubleClickStrawberry);
   guishEH->getSignal(TreeNode, OSGUIsh::EVENT_DOUBLE_CLICK)
      ->connect(&HandleDoubleClickTree);

   // Enter rendering loop
   viewer.run();
}
