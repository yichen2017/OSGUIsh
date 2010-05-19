/******************************************************************************\
* EventHandler.cpp                                                             *
* An event handler providing GUI-like events                                   *
*                                                                              *
* Copyright (C) 2006-2010 by Leandro Motta Barros.                             *
*                                                                              *
* This program is distributed under the OpenSceneGraph Public License. You     *
* should have received a copy of it with the source distribution, in a file    *
* named 'COPYING.txt'.                                                         *
\******************************************************************************/

#include "OSGUIsh/EventHandler.hpp"
#include <boost/lexical_cast.hpp>

#include <iostream> /////////////////////////////////////////////////////////////////////////////


namespace OSGUIsh
{
   // - EventHandler::EventHandler ---------------------------------------------
   EventHandler::EventHandler(
      const FocusPolicyFactory& kbdPolicyFactory,
      const FocusPolicyFactory& wheelPolicyFactory)
      : ignoreBackFaces_(false),
        kbdFocusPolicy_(kbdPolicyFactory.create (kbdFocus_)),
        wheelFocusPolicy_(wheelPolicyFactory.create (wheelFocus_))
   {
      addNode (NodePtr());

      for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i)
      {
         nodeThatGotMouseDown_[i] = NodePtr();
         nodeThatGotClick_[i] = NodePtr();
         timeOfLastClick_[i] = -1.0;
      }

      // // // // // By default, use the viewer's scene root as the only root node when
      // // // // // picking
      // // // // pickingRoots_.push_back (viewer_.getSceneData());
   }



   // - EventHandler::handle ---------------------------------------------------
   bool EventHandler::handle(const osgGA::GUIEventAdapter& ea,
                             osgGA::GUIActionAdapter& aa)
   {
      switch (ea.getEventType())
      {
         case osgGA::GUIEventAdapter::FRAME:
            handleFrameEvent(dynamic_cast<osgViewer::View*>(&aa), ea);
            break;

         case osgGA::GUIEventAdapter::PUSH:
            handlePushEvent(ea);
            break;

         case osgGA::GUIEventAdapter::RELEASE:
            handleReleaseEvent(ea);
            break;

         case osgGA::GUIEventAdapter::KEYDOWN:
            handleKeyDownEvent(ea);
            break;

         case osgGA::GUIEventAdapter::KEYUP:
            handleKeyUpEvent(ea);
            break;

         case osgGA::GUIEventAdapter::SCROLL:
            handleScrollEvent(ea);
            break;

         default:
            break;
      }

      kbdFocusPolicy_->updateFocus(ea, nodeUnderMouse_);
      wheelFocusPolicy_->updateFocus(ea, nodeUnderMouse_);

      return handleReturnValues_[ea.getEventType()];
   }



   // // // // // - EventHandler::setPickingRoots ------------------------------------------
   // // // // void EventHandler::setPickingRoots (std::vector<NodePtr> newRoots)
   // // // // {
   // // // //    pickingRoots_ = newRoots;
   // // // // }



   // // // // // - EventHandler::setPickingRoot -------------------------------------------
   // // // // void EventHandler::setPickingRoot (NodePtr newRoot)
   // // // // {
   // // // //    std::vector<NodePtr> newRoots;
   // // // //    newRoots.push_back (newRoot);
   // // // //    setPickingRoots (newRoots);
   // // // // }



   // - EventHandler::addNode --------------------------------------------------
   void EventHandler::addNode(const osg::ref_ptr<osg::Node> node)
   {
#     define OSGUISH_EVENTHANDLER_ADD_EVENT(EVENT) \
         signals_[node][#EVENT] = SignalPtr(new EventHandler::Signal_t());

      OSGUISH_EVENTHANDLER_ADD_EVENT(MouseEnter);
      OSGUISH_EVENTHANDLER_ADD_EVENT(MouseLeave);
      OSGUISH_EVENTHANDLER_ADD_EVENT(MouseMove);
      OSGUISH_EVENTHANDLER_ADD_EVENT(MouseDown);
      OSGUISH_EVENTHANDLER_ADD_EVENT(MouseUp);
      OSGUISH_EVENTHANDLER_ADD_EVENT(Click);
      OSGUISH_EVENTHANDLER_ADD_EVENT(DoubleClick);
      OSGUISH_EVENTHANDLER_ADD_EVENT(MouseWheelUp);
      OSGUISH_EVENTHANDLER_ADD_EVENT(MouseWheelDown);
      OSGUISH_EVENTHANDLER_ADD_EVENT(KeyUp);
      OSGUISH_EVENTHANDLER_ADD_EVENT(KeyDown);

#     undef OSGUISH_EVENTHANDLER_ADD_EVENT
   }


   // - EventHandler::getSignal ------------------------------------------------
   EventHandler::SignalPtr EventHandler::getSignal(
      const NodePtr node, const std::string& signal)
   {
       SignalsMap_t::const_iterator signalsCollectionIter =
          signals_.find (node);

      if (signalsCollectionIter == signals_.end())
      {
         throw std::runtime_error(
            ("Trying to get a signal of an unknown node: '" + node->getName()
             + "' (" + boost::lexical_cast<std::string>(node.get())
             + ").").c_str());
      }

      SignalCollection_t::const_iterator signalIter =
         signalsCollectionIter->second.find (signal);

      if (signalIter == signalsCollectionIter->second.end())
      {
         throw std::runtime_error (("Trying to get an unknown signal: '"
                                    + signal + "'.").c_str());
      }

      return signalIter->second;
   }



   // - EventHandler::setKeyboardFocus -----------------------------------------
   void EventHandler::setKeyboardFocus(const NodePtr node)
   {
      kbdFocus_ = node;
   }



   // - EventHandler::setMouseWheelFocus ---------------------------------------
   void EventHandler::setMouseWheelFocus(const NodePtr node)
   {
      wheelFocus_ = node;
   }



   // - EventHandler::setKeyboardFocusPolicy -----------------------------------
   void EventHandler::setKeyboardFocusPolicy(
      const FocusPolicyFactory& policyFactory)
   {
      kbdFocusPolicy_ = policyFactory.create(kbdFocus_);
   }



   // - EventHandler::setMouseWheelFocusPolicy ---------------------------------
   void EventHandler::setMouseWheelFocusPolicy(
      const FocusPolicyFactory& policyFactory)
   {
      wheelFocusPolicy_ = policyFactory.create(wheelFocus_);
   }



   // - EventHandler::getObservedNode ------------------------------------------
   NodePtr EventHandler::getObservedNode(const osg::NodePath& nodePath)
   {
      typedef osg::NodePath::const_reverse_iterator iter_t;
      for (iter_t p = nodePath.rbegin(); p != nodePath.rend(); ++p)
      {
         if (signals_.find (NodePtr(*p)) != signals_.end())
            return NodePtr(*p);
      }

      return NodePtr();
   }



   // - EventHandler::handleFrameEvent -----------------------------------------
   void EventHandler::handleFrameEvent(osgViewer::View* view,
                                       const osgGA::GUIEventAdapter& ea)
   {
      // // // assert (pickingRoots_.size() > 0);

      // Find out who is, and who was under the mouse pointer
      NodePtr currentNodeUnderMouse;
      osg::Vec3 currentPositionUnderMouse;

      // // // // typedef std::vector <NodePtr>::iterator iter_t;
      // // // // for (iter_t p = pickingRoots_.begin(); p != pickingRoots_.end(); ++p)
      // // // // {
         // // // // osgUtil::IntersectVisitor::HitList hitList;
         osgUtil::LineSegmentIntersector::Intersections hitList;
         //////////// can pass the nodepath (to support HUDs/multiple roots)
         view->computeIntersections(ea.getX(), ea.getY(), hitList);

         if (hitList.size() > 0)
         {
            typedef
               osgUtil::LineSegmentIntersector::Intersections::const_iterator
               iter_t;

            iter_t theHit = hitList.end();

            // This implementation needs two or more hits to correctly ignore
            // back faces. There is a little more detail about this in the
            // Doxygen comments for \c ignoreBackFaces().
            if (ignoreBackFaces_ && hitList.size() >= 2)
            {
               const osg::Vec3 begin =
                  hitList.begin()->getWorldIntersectPoint();
               const osg::Vec3 end =
                  (--hitList.end())->getWorldIntersectPoint();

               osg::Vec3 rayDir = end - begin;
               rayDir.normalize();

               for (iter_t hit = hitList.begin(); hit != hitList.end(); ++hit)
               {
                  const bool frontFacing =
                     rayDir * hit->getWorldIntersectNormal() < 0.0;

                  if (frontFacing)
                  {
                     theHit = hit;
                     break;
                  }
               }
            }
            else // !ignoreBackFaces_
            {
               theHit = hitList.begin();
            }

            if (theHit != hitList.end())
            {
               currentNodeUnderMouse = getObservedNode (theHit->nodePath);
               assert (signals_.find (currentNodeUnderMouse) != signals_.end()
                       && "'getObservedNode()' returned an invalid value!");

               currentPositionUnderMouse = theHit->getLocalIntersectPoint();

               hitUnderMouse_ = *theHit;

               // // // // // break;
            }
         }  // if (hitList.size() > 0)
      // // // // } // for (...pickingRoots_...)

      NodePtr prevNodeUnderMouse = nodeUnderMouse_;
      osg::Vec3 prevPositionUnderMouse = positionUnderMouse_;

      nodeUnderMouse_ = currentNodeUnderMouse;
      positionUnderMouse_ = currentPositionUnderMouse;

      // Trigger the events
      if (currentNodeUnderMouse == prevNodeUnderMouse)
      {
         if (prevNodeUnderMouse.valid()
             && currentPositionUnderMouse != prevPositionUnderMouse)
         {
            HandlerParams params (currentNodeUnderMouse, ea, hitUnderMouse_);
            signals_[currentNodeUnderMouse]["MouseMove"]->operator()(params);
         }
      }
      else // currentNodeUnderMouse != prevNodeUnderMouse
      {
         if (prevNodeUnderMouse.valid())
         {
            HandlerParams params (prevNodeUnderMouse, ea, hitUnderMouse_);
            signals_[prevNodeUnderMouse]["MouseLeave"]->operator()(params);
         }

         if (currentNodeUnderMouse.valid())
         {
            HandlerParams params (currentNodeUnderMouse, ea, hitUnderMouse_);
            signals_[currentNodeUnderMouse]["MouseEnter"]->operator()(params);
         }
      }
   }



   // - EventHandler::handlePushEvent ------------------------------------------
   void EventHandler::handlePushEvent(const osgGA::GUIEventAdapter& ea)
   {
      // Trigger a "MouseDown" signal.
      if (nodeUnderMouse_.valid())
      {
         HandlerParams params (nodeUnderMouse_, ea, hitUnderMouse_);
         signals_[nodeUnderMouse_]["MouseDown"]->operator()(params);
      }

      // Do the bookkeeping for "Click" and "DoubleClick"
      MouseButton button = getMouseButton (ea);
      nodeThatGotMouseDown_[button] = nodeUnderMouse_;
   }



   // - EventHandler::handleReleaseEvent ---------------------------------------
   void EventHandler::handleReleaseEvent(const osgGA::GUIEventAdapter& ea)
   {
      const double DOUBLE_CLICK_INTERVAL = 0.3;

      if (nodeUnderMouse_.valid())
      {
         MouseButton button = getMouseButton (ea);

         // First the trivial case: the "MouseUp" event
         HandlerParams params(nodeUnderMouse_, ea, hitUnderMouse_);
         signals_[nodeUnderMouse_]["MouseUp"]->operator()(params);

         // Now, the trickier ones: "Click" and "DoubleClick"
         if (nodeUnderMouse_ == nodeThatGotMouseDown_[button])
         {
            HandlerParams params(nodeUnderMouse_, ea, hitUnderMouse_);
            signals_[nodeUnderMouse_]["Click"]->operator()(params);

            const double now = ea.getTime();

            if (now - timeOfLastClick_[button] < DOUBLE_CLICK_INTERVAL
                && nodeUnderMouse_ == nodeThatGotClick_[button])
            {
               HandlerParams params (nodeUnderMouse_, ea, hitUnderMouse_);
               signals_[nodeUnderMouse_]["DoubleClick"]->operator()(params);
            }

            nodeThatGotClick_[button] = nodeUnderMouse_;
            timeOfLastClick_[button] = now;
         }
      }
   }



   // - EventHandler::handleKeyDownEvent ---------------------------------------
   void EventHandler::handleKeyDownEvent(const osgGA::GUIEventAdapter& ea)
   {
      HandlerParams params(kbdFocus_, ea, hitUnderMouse_);
      signals_[kbdFocus_]["KeyDown"]->operator()(params);
   }



   // - EventHandler::handleKeyUpEvent -----------------------------------------
   void EventHandler::handleKeyUpEvent(const osgGA::GUIEventAdapter& ea)
   {
      HandlerParams params(kbdFocus_, ea, hitUnderMouse_);
      signals_[kbdFocus_]["KeyUp"]->operator()(params);
   }



   // - EventHandler::handleScrollEvent ----------------------------------------
   void EventHandler::handleScrollEvent(const osgGA::GUIEventAdapter& ea)
   {
      switch (ea.getScrollingMotion())
      {
         case osgGA::GUIEventAdapter::SCROLL_UP:
         {
            HandlerParams params(wheelFocus_, ea, hitUnderMouse_);
            signals_[wheelFocus_]["MouseWheelUp"]->operator()(params);
            break;
         }

         case osgGA::GUIEventAdapter::SCROLL_DOWN:
         {
            HandlerParams params(wheelFocus_, ea, hitUnderMouse_);
            signals_[wheelFocus_]["MouseWheelDown"]->operator()(params);
            break;
         }

         default:
            break; // ignore other events
      }
   }



   // - EventHandler::getMouseButton -------------------------------------------
   EventHandler::MouseButton EventHandler::getMouseButton(
      const osgGA::GUIEventAdapter& ea)
   {
      switch (ea.getButton())
      {
         case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
            return LEFT_MOUSE_BUTTON;
         case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON:
            return MIDDLE_MOUSE_BUTTON;
         case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
            return RIGHT_MOUSE_BUTTON;
         default:
         {
            assert(false && "Got an invalid mouse button code. Is 'ea' really "
                   "a mouse event?");
         }
      }
   }

} // namespace OSGUIsh
