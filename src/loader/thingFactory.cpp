/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>
#include <memory>

#include "debug.h"

#include "thingFactory.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "zztEntity.h"
#include "zztThing.h"
#include "enemies.h"
#include "zztStructs.h"

using namespace ZZTThing;

class ThingFactoryPrivate
{
  public:
    ThingFactoryPrivate( ThingFactory *pSelf )
      : self( pSelf )
    { /* */ };

    AbstractThing *createThing( const ZZTEntity &entity, const ThingHeader& header );
    void prepareThing( AbstractThing *thing, const ThingHeader& header );

    Player *createPlayer( const ThingHeader& header );
    Scroll *createScroll( const ThingHeader& header );
    Passage *createPassage( const ThingHeader& header );
    Duplicator *createDuplicator( const ThingHeader& header );
    Bear *createBear( const ThingHeader& header );
    Ruffian *createRuffian( const ThingHeader& header );
    Object *createObject( const ThingHeader& header );
    Slime *createSlime( const ThingHeader& header );
    Shark *createShark( const ThingHeader& header );
    SpinningGun *createSpinningGun( const ThingHeader& header );
    Pusher *createPusher( const ThingHeader& header );
    Lion *createLion( const ThingHeader& header );
    Tiger *createTiger( const ThingHeader& header );
    CentipedeHead *createCentipedeHead( const ThingHeader& header );
    CentipedeSegment *createCentipedeSegment( const ThingHeader& header );
    BlinkWall *createBlinkWall( const ThingHeader& header );
    Transporter *createTransporter( const ThingHeader& header );
    Bullet *createBullet( const ThingHeader& header );
    Star *createStar( const ThingHeader& header );

  public:
    GameWorld *world;
    GameBoard *board;

  private:
    ThingFactory *self;
};

AbstractThing * ThingFactoryPrivate::createThing( const ZZTEntity &entity,
                                                  const ThingHeader& header )
{
  AbstractThing *thing = 0;
  switch ( entity.id() )
  {
    case ZZTEntity::Player:
      thing = createPlayer( header );
      break;

    case ZZTEntity::Scroll:
      thing = createScroll( header );
      break;

    case ZZTEntity::Passage:
      thing = createPassage( header );
      break;

    case ZZTEntity::Duplicator:
      thing = createDuplicator( header );
      break;

    case ZZTEntity::Bear:
      thing = createBear( header );
      break;

    case ZZTEntity::Ruffian:
      thing = createRuffian( header );
      break;

    case ZZTEntity::Object:
      thing = createObject( header );
      break;

    case ZZTEntity::Slime:
      thing = createSlime( header );
      break;

    case ZZTEntity::Shark:
      thing = createShark( header );
      break;

    case ZZTEntity::SpinningGun:
      thing = createSpinningGun( header );
      break;

    case ZZTEntity::Pusher:
      thing = createPusher( header );
      break;

    case ZZTEntity::Lion:
      thing = createLion( header );
      break;

    case ZZTEntity::Tiger:
      thing = createTiger( header );
      break;

    case ZZTEntity::CentipedeHead:
      thing = createCentipedeHead( header );
      break;

    case ZZTEntity::CentipedeSegment:
      thing = createCentipedeSegment( header );
      break;

    case ZZTEntity::BlinkWall:
      thing = createBlinkWall( header );
      break;

    case ZZTEntity::Transporter:
      thing = createTransporter( header );
      break;

    case ZZTEntity::Bullet:
      thing = createBullet( header );
      break;

    case ZZTEntity::Star:
      thing = createStar( header );
      break;

    default: break;
  }

  prepareThing( thing, header );
  return thing;
}

void ThingFactoryPrivate::prepareThing( AbstractThing *thing,
                                        const ThingHeader& header )
{
  if (!thing) return;

  thing->setWorld( world );
  thing->setBoard( board );
  thing->setPos( header.x-1, header.y-1 );
  thing->setCycle( header.cycle );

  ZZTEntity ent = ZZTEntity::createEntity( header.underTile, header.underColor );
  thing->setUnderEntity( ent );
}

Player * ThingFactoryPrivate::createPlayer( const ThingHeader& header )
{
  Player *player = new Player();
  return player;
}

Scroll * ThingFactoryPrivate::createScroll( const ThingHeader& header )
{
  Scroll *scroll = new Scroll();
  return scroll;
}

Passage * ThingFactoryPrivate::createPassage( const ThingHeader& header )
{
  Passage *passage = new Passage();
  return passage;
}

Duplicator * ThingFactoryPrivate::createDuplicator( const ThingHeader& header )
{
  Duplicator *duplicator = new Duplicator();
  return duplicator;
}

Bear * ThingFactoryPrivate::createBear( const ThingHeader& header )
{
  Bear *bear = new Bear();
  bear->setSensativity( header.param1 );
  return bear;
}

Ruffian * ThingFactoryPrivate::createRuffian( const ThingHeader& header )
{
  Ruffian *ruffian = new Ruffian();
  ruffian->setIntelligence( header.param1 );
  ruffian->setRest( header.param2 );
  return ruffian;
}

Object * ThingFactoryPrivate::createObject( const ThingHeader& header )
{
  return new Object();
}

Slime * ThingFactoryPrivate::createSlime( const ThingHeader& header )
{
  Slime *slime = new Slime();
  slime->setSpeed( header.param2 );
  return slime;
}

Shark * ThingFactoryPrivate::createShark( const ThingHeader& header )
{
  Shark *shark = new Shark();
  shark->setIntelligence( header.param1 );
  return shark;
}

SpinningGun * ThingFactoryPrivate::createSpinningGun( const ThingHeader& header )
{
  return new SpinningGun();
}

Pusher * ThingFactoryPrivate::createPusher( const ThingHeader& header )
{
  return new Pusher();
}

Lion * ThingFactoryPrivate::createLion( const ThingHeader& header )
{
  Lion *lion = new Lion();
  lion->setIntelligence( header.param1 );
  return lion;
}

Tiger * ThingFactoryPrivate::createTiger( const ThingHeader& header )
{
  Tiger *tiger = new Tiger();
  tiger->setIntelligence( header.param1 );
  return tiger;
}

CentipedeHead * ThingFactoryPrivate::createCentipedeHead( const ThingHeader& header )
{
  CentipedeHead *head = new CentipedeHead();
  head->setIntelligence( header.param1 );
  head->setDeviance( header.param2 );

  zdebug() << "CENTIPEDE HEAD"
           << " dx:" << (int) header.x_step
           << " dy:" << (int) header.y_step
           << " cy:" << (int) header.cycle
           << " p1:" << (int) header.param1
           << " p2:" << (int) header.param2
           << " p3:" << (int) header.param3
           << " p4:" << (int) header.param4;

  return head;
}

CentipedeSegment * ThingFactoryPrivate::createCentipedeSegment( const ThingHeader& header )
{
  zdebug() << "CENTIPEDE SEGMENT"
           << " dx:" << (int) header.x_step
           << " dy:" << (int) header.y_step
           << " cy:" << (int) header.cycle
           << " p1:" << (int) header.param1
           << " p2:" << (int) header.param2
           << " p3:" << (int) header.param3
           << " p4:" << (int) header.param4;

  return new CentipedeSegment();
}

BlinkWall * ThingFactoryPrivate::createBlinkWall( const ThingHeader& header )
{
  return new BlinkWall();
}

Transporter * ThingFactoryPrivate::createTransporter( const ThingHeader& header )
{
  return new Transporter();
}

Bullet * ThingFactoryPrivate::createBullet( const ThingHeader& header )
{
  return new Bullet();
}

Star * ThingFactoryPrivate::createStar( const ThingHeader& header )
{
  return new Star();
}

// -----------------------------------------------------------------------------

ThingFactory::ThingFactory()
  : d( new ThingFactoryPrivate(this) )
{
  /* */
}

GameWorld *ThingFactory::world() const
{
  return d->world;
}

void ThingFactory::setWorld( GameWorld *world )
{
  d->world = world;
}

GameBoard *ThingFactory::board() const
{
  return d->board;
}

void ThingFactory::setBoard( GameBoard *board )
{
  d->board = board;
}

AbstractThing * ThingFactory::createThing( const unsigned char *data, int &thingSize )
{
  std::auto_ptr<ThingHeader> header( new ThingHeader(data) );

  // Really ZZT file format, I have to look at the board to get the
  // entity number? Why didn't you encode it, you had 7 unused bytes there.
  ZZTEntity entity = d->board->entity( header->x - 1, header->y - 1 );

  thingSize = 0x21 + header->programLength;

  return d->createThing( entity, *header );
}

