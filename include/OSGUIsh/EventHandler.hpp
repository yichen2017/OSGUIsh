/******************************************************************************\
* EventHandler.hpp                                                             *
* An event handler providing GUI-like events                                   *
* Leandro Motta Barros                                                         *
\******************************************************************************/

#ifndef _GUISH_EVENT_HANDLER_HPP_
#define _GUISH_EVENT_HANDLER_HPP_


#include <boost/signal.hpp>
#include <osgProducer/Viewer>


namespace OSGUIsh
{

   /// A (smart) pointer to a scene graph node.
   typedef osg::ref_ptr<osg::Node> NodePtr;


   /** An event handler providing GUI-like events for nodes. The \c EventHandler
    *  has an internal list of nodes being "observed". Every observed node has a
    *  collection of signals associated to it. These signals represent the
    *  events that can be generated for the node.
    */
   class EventHandler: public osgGA::GUIEventHandler
   {
      public:
         /** Constructs an \c EventHandler.
          *  @param viewer The viewer used to view the scene graph. This used
          *         for calling its \c computeIntersections() method.
          */
         EventHandler (osgProducer::Viewer& viewer);

         /** Handle upcoming events (overloads virtual method).
          *  @return See \c handleReturnValues_ please.
          */
         bool handle (const osgGA::GUIEventAdapter& ea,
                      osgGA::GUIActionAdapter&);

         /** A type representing a signal used in OSGUIsh. This signal returns
          *  nothing and takes two parameters: the \c osgGA::GUIEventAdapter
          *  that generated this event and a pointer to the node receiving the
          *  event.
          */
         typedef boost::signal<void (const osgGA::GUIEventAdapter&,
                                     NodePtr)> Signal_t;

         /// A (smart) pointer to a \c Signal_t;
         typedef boost::shared_ptr<Signal_t> SignalPtr;

         /** Adds a given node to the list of nodes being "observed" by this
          *  \c EventHandler. In other words, signals for this node will be
          *  triggered from this call on.
          *  @param node The node that will be added to this \c EventHandler.
          */
         void addNode (const NodePtr node);

         /** Returns a signal associated with a given node. This is typically
          *  used to call \c connect() on the returned signal.
          *  @param node The desired node.
          *  @param signal The desired signal. Valid signals are
          *         <tt>"MouseEnter"</tt>, <tt>"MouseLeave"</tt>,
          *         <tt>"MouseMove"</tt>, <tt>"MouseDown"</tt>,
          *         <tt>"MouseUp"</tt>, <tt>"Click"</tt>,
          *         <tt>"DoubleClick"</tt>, <tt>"MouseWheelUp"</tt>,
          *         <tt>"MouseWheelDown"</tt>, <tt>"KeyUp"</tt> and
          *         <tt>"KeyDown"</tt>.
          */
         SignalPtr getSignal (const NodePtr node, const std::string& signal);

      private:
         /** Returns the first node in an \c osg::NodePath that is present in the
          *  list of nodes being "observed" by this \c EventHandler. This is
          *  necessary in the cases in which the user is picking a node that is
          *  child of an added node. That's the case, for instance, of when the
          *  user adds a node read from a 3D model file returned by
          *  \c osgDB::readNodeFile().
          *  @param nodePath The node path leading to the node being queried.
          *  @returns The first in \c nodePath that as added to the list of
          *           nodes being observed.
          *  @todo The node path is being traversed from begin to end. Shouldn't
          *        it be the opposite?
          */
         NodePtr getObservedNode (const osg::NodePath& nodePath);

         /// The viewer viewing the nodes.
         osgProducer::Viewer& viewer_;

         /** The list of nodes under the mouse pointer. This includes \e all
          *  nodes under the mouse, not only the nodes being observed by this
          *  \c EventHandler.
          */
         osgUtil::IntersectVisitor::HitList hitList_;

         /** The values to be returned by the \c handle() method, depending on
          *  the event type it is handling. Values are not initialized, meaning
          *  that \c handle(), by default, returns \c false for all event types.
          *  @todo Implement a method like \c setHandleReturnValue(), so that
          *        the user can configure this.
          */
         std::map <osgGA::GUIEventAdapter::EventType, bool> handleReturnValues_;

         /// Type mapping a signal name to the signal object.
         typedef std::map <std::string, SignalPtr> SignalCollection_t;

         /// Type mapping nodes to the collection of signals associated to it.
         typedef std::map <NodePtr, SignalCollection_t > SignalsMap_t;

         /** Structure containing all the signals used by this \c EventHandler.
          *  It can be used with a syntax like this:
          *  <tt>signals_[node]["MouseMove"]->connect(&MyHandler);</tt>
          */
         SignalsMap_t signals_;

         //
         // For "MouseEnter", "MouseLeave", "MouseMove"
         //

         /// The node currently under the mouse pointer.
         NodePtr nodeUnderMouse_;

         /** The position (in the object coordinate system) of
          *  \c nodeUnderMouse_ currently under the mouse pointer.
          */
         osg::Vec3 positionUnderMouse_;
   };

} // namespace OSGUIsh


#endif // _GUISH_EVENT_HANDLER_HPP_
