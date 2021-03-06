/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cstdlib>
#include <cassert>
#include <list>
#include <vector>
#include <algorithm>

#include "debug.h"
#include "zstring.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"

#include "zztThing.h"
#include "enemies.h"

using namespace ZZTThing;

// -------------------------------------

Bear::Bear()
  : m_paramSensativity(0)
{
  /* */
}

void Bear::exec_impl()
{
  /* */
}

// -------------------------------------

Ruffian::Ruffian()
  : m_paramIntel(0),
    m_paramRest(0),
    m_moves(0),
    m_rests(0)
{
  /* */
}

void Ruffian::exec_impl()
{
  if ( m_rests == 0 ) {
    m_moves = rand() % 10;
    m_rests = m_paramRest;
    if ( rand() % 9 < m_paramIntel ) {
      m_direction = seekDir();
    }
    else {
      m_direction = randAnyDir();
    }
  }

  if ( m_moves > 0 ) {
    doMove( m_direction );
    m_moves -= 1;
  }
  else if ( m_rests > 0 ) {
    m_rests -= 1;
  }
}

// -------------------------------------

Slime::Slime()
{
  /* */
}

void Slime::exec_impl()
{
  /* */
}

// -------------------------------------

Shark::Shark()
  : m_paramIntel(0)
{
  /* */
}

void Shark::exec_impl()
{
  /* */
}

// -------------------------------------

Lion::Lion()
  : m_paramIntel(0)
{
  /* */
}

void Lion::exec_impl()
{
  doMove( randAnyDir() );
}

// -------------------------------------

Tiger::Tiger()
  : m_paramIntel(0)
{
  /* */
}

void Tiger::exec_impl()
{
  doMove( randAnyDir() );
}

// -------------------------------------

// reference notes:
// when surrounded by too many segments, the head can't decide and the
// segments become heads

CentipedeHead::CentipedeHead()
  : m_paramIntel(0),
    m_paramDeviance(0),
    m_direction(0),
    m_settleTime(2)
{
  /* */
}

void CentipedeHead::exec_impl()
{
  if ( m_settleTime )
  {
    findSegments();
    m_settleTime -= 1;
  }
  else {
    int oldX = xPos();
    int oldY = yPos();

    doMove( randNotBlockedDir() );

    if ( oldX == xPos() && oldY == yPos() ) {
      switchHeadAndTail();
    }
    else {
      moveSegments( oldX, oldY );
    }
  }
}


static void checkAddSegment( const ZZTEntity &ent,
                             std::vector<CentipedeSegment*> &segs )
{
  if ( ent.id() != ZZTEntity::CentipedeSegment ) return;
  segs.push_back( static_cast<CentipedeSegment*>(ent.thing()) );
}

void CentipedeHead::findSegments()
{
  int x, y;
  if ( m_body.empty() ) {
    x = xPos();
    y = yPos();
  }
  else {
    CentipedeSegment *segment = m_body.back();
    x = segment->xPos();
    y = segment->yPos();
  }

  bool addedBody;
  do
  {
    std::vector<CentipedeSegment*> segs;
    segs.reserve(4);
    checkAddSegment( board()->entity( x, y-1 ), segs );
    checkAddSegment( board()->entity( x, y+1 ), segs );
    checkAddSegment( board()->entity( x-1, y ), segs );
    checkAddSegment( board()->entity( x+1, y ), segs );

    const int segCount = segs.size();
    if ( segCount == 0 || segCount == 4 ) break;
    if ( segCount > 1 ) {
      random_shuffle ( segs.begin(), segs.end() );
    }

    addedBody = false;
    std::vector<CentipedeSegment*>::iterator iter;
    for ( iter = segs.begin(); iter != segs.end(); iter++ )
    {
      CentipedeSegment *segment = *iter;
      if ( !segment->head() ) {
        segment->setHead(this);
        m_body.push_back(segment);
        x = segment->xPos();
        y = segment->yPos();
        addedBody = true;
      }
    }
  }
  while ( addedBody );
}

void CentipedeHead::moveSegments( int oldX, int oldY )
{
  CentipedeBody::iterator iter;
  for ( iter = m_body.begin(); iter != m_body.end(); iter++ )
  {
    CentipedeSegment *segment = *iter;
    const int newX = oldX, newY = oldY;
    oldX = segment->xPos();
    oldY = segment->yPos();
    board()->moveThing( segment, newX, newY );
  }
}

void CentipedeHead::switchHeadAndTail()
{
  if (m_body.empty()) return;

  // in which a tail becomes a head and a head becomes a tail
  CentipedeSegment *tail = m_body.back();
  m_body.pop_back();
  m_body.reverse();
  m_body.push_back( tail );
  board()->switchThings( this, tail );
}

void CentipedeHead::handleDeleted()
{
  CentipedeBody::iterator iter;
  for ( iter = m_body.begin(); iter != m_body.end(); iter++ ) {
    CentipedeSegment *segment = *iter;
    segment->setHead(0);
  }
  m_body.clear();
}

void CentipedeHead::handleLosingSegment( CentipedeSegment *segment )
{
  // cut the body short at the point that the segment was removed.
  CentipedeBody lostBody;
  CentipedeSegment *lostSegment = 0;

  while ( !m_body.empty() )
  {
    lostSegment = m_body.back();
    m_body.pop_back();

    if ( lostSegment == segment ) break;

    // only clear the "head" from the living segments, so other
    // heads don't try to connect to this dead segment. (segfault city)
    lostSegment->setHead(0);

    lostBody.push_front(lostSegment);
  }

  if (!lostBody.empty()) {
    lostBody.front()->becomeHead();
  }
}

// -------------------------------------

CentipedeSegment::CentipedeSegment()
  : m_head(0),
    m_isolated(0)
{
  /* */
}

void CentipedeSegment::exec_impl()
{
  const bool haveHead = (head());
  m_isolated = haveHead ? 0 : m_isolated + 1;

  if (m_isolated > 2) {
    const int x = xPos(), y = yPos();
    int count = 0;
    if ( board()->entity( x, y-1 ).id() == ZZTEntity::CentipedeSegment ) count++;
    if ( board()->entity( x, y+1 ).id() == ZZTEntity::CentipedeSegment ) count++;
    if ( board()->entity( x-1, y ).id() == ZZTEntity::CentipedeSegment ) count++;
    if ( board()->entity( x+1, y ).id() == ZZTEntity::CentipedeSegment ) count++;
    if ( count <= 1 ) {
      // Only become a head if not part of a visible chain
      becomeHead();
    }
  }
}

void CentipedeSegment::becomeHead()
{
  assert( m_head == 0 );

  const int x = xPos(), y = yPos();
  const ZZTEntity myEnt = board()->entity( x, y );

  // create new CentipedeHead to take my place.
  ZZTEntity headEnt = ZZTEntity::createEntity( ZZTEntity::CentipedeHead, myEnt.color() );
  CentipedeHead *newHead = new CentipedeHead();
  newHead->setBoard( board() );
  newHead->setXPos( x );
  newHead->setYPos( y );
  newHead->setCycle( cycle() );
  newHead->setIntelligence(1);
  newHead->setDeviance(1);
  
  // become board garbage.
  board()->deleteThing( this );

  // add the new me.
  newHead->setUnderEntity( board()->entity( x, y ) );
  board()->setEntity( x, y, headEnt );
  board()->addThing( newHead );
  newHead->findSegments();
}

void CentipedeSegment::handleDeleted()
{
  if (head()) {
    head()->handleLosingSegment(this);
  }
}

