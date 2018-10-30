#include<cstdint>
#include<cstdio>
#include<cassert>
#include<memory>

using namespace std;

#define VSPRINTF(...) \
    do { \
        printf("L%d %s(): ", __LINE__, __func__); \
        printf(__VA_ARGS__); \
    } while (false);\

class Entity {
public:
    Entity(int id)
        : id_(id)
    {
        VSPRINTF("new entity created (%d)\n", id_);
    }

    ~Entity()
    {
        VSPRINTF("entity %d destroyed\n", id_);
    }

    bool isOnSurface() const;
    void accelerate(int a);
    void update();

private:
    int id_;
};


enum Event {
    EVENT_ENTITY_FELL = 0,
    // ...
};

/***************************************************************/

class Observer
{
public:
    virtual ~Observer() {}
    virtual void onNotify(const Entity& entity,
            Event event) = 0;
};

/***************************************************************/
/*
   Achievement is one of observer
*/

class Achievement : public Observer
{
public:
    Achievement()
        : heroIsOnBride_(false)
    {}

    virtual void onNotify(const Entity& entity,
            Event event);

    void setHeroIsOnBridge(bool en) { heroIsOnBride_ = en; }

private:
    void unlock(Achievement achievement);

    bool heroIsOnBride_;
};

void Achievement::onNotify(const Entity& entity, Event event)
{
    switch (event)
    {
    case EVENT_ENTITY_FELL:
        printf("EVENT_ENTITY_FELL called\n");
        break;

    default:
        break;
    }
}

void Achievement::unlock(Achievement achievement)
{
}

/***************************************************************/

class Subject
{
public:
    Subject()
        : numObservers_(0)
    {
        for (int i = 0; i < kMaxObservers; i++)
        {
            observers_[i] = nullptr;
        }

        createEntity();
    }

    ~Subject()
    {
        assert(entity_);
        delete (entity_);
    }

    void addObserver(Observer* observer);
    void removeObserver(const Observer* observer);
    void fall();

protected:
    void notify(const Entity& entity, Event event);

private:
    static const int kMaxObservers = 10;

    void createEntity();

    Observer* observers_[kMaxObservers];
    int numObservers_;
    Entity* entity_;

    static uint32_t numEntity_;
};

void Subject::createEntity()
{
    static int numEntity = 0;

    entity_ = new Entity(numEntity);
    numEntity++;
}

void Subject::addObserver(Observer* observer)
{
    for (int i = 0; i < kMaxObservers; i++)
    {
        if (observers_[i] == nullptr)
        {
            observers_[i] = observer;
            return;
        }
    }

    assert(false && "cannot add observer\n");
}

void Subject::removeObserver(const Observer* observer)
{
    for (int i = 0; i < kMaxObservers; i++)
    {
        if (observers_[i] == observer)
        {
            observers_[i] = nullptr;
        }
    }
}

void Subject::fall()
{
    // do something to fall

    notify(*entity_, Event::EVENT_ENTITY_FELL);

}

void Subject::notify(const Entity& entity, Event event)
{
    VSPRINTF("");
    for (int i = 0; i < numObservers_; i++)
    {
        if (observers_[i])
            observers_[i]->onNotify(entity, event);
    }
}


/***************************************************************/

int main()
{
    Achievement achievementA, achievementB;

    Subject subjectA;
    subjectA.addObserver(&achievementA);
    subjectA.addObserver(&achievementB);
    subjectA.fall();

    return 0;
}

// todo: check why subject::fall() never called?
