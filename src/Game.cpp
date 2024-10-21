#include "Game.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>


Game::Game(const std::string &config)
{
    init(config);
}

void Game::init(const std::string &path)
{
    //random number seed generator
    srand(time(NULL));

    //TODO: read in config file
    //use premade PlayerConfig, EnemyConfig, BulletConfig variables
    std::string title;
    std::ifstream fin(path);

    int wWidth;
    int wHeight;
    int fps;
    bool fullscreen;
    std::string fontName;
    int fontSize;
    int fontR, fontG, fontB;

    while (fin >> title)
    {
        if (title == "Window")
        {
            std::cout << "Found Window!\n";
            fin >> wWidth >> wHeight >> fps >> fullscreen;
            if (fullscreen)
                m_window.create(sf::VideoMode(wWidth, wHeight), "Bean Game", sf::Style::Fullscreen);
            else
                m_window.create(sf::VideoMode(wWidth, wHeight), "Bean Game");
        }
        if (title == "Font")
        {
            fin >> fontName;
            if (!m_font.loadFromFile(fontName))
            {
                std::cerr << "Could not load font!\n";
                exit(-1);
	        }
            std::cout << "Font Loaded\n";
            fin >> fontSize >> fontR >> fontG >> fontB;
        }
        if (title == "Player")
        {
            std::cout << "Found Player!\n";
            fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S
                >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB
                >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
                >> m_playerConfig.OT >> m_playerConfig.V;
        }
        if (title == "Enemy")
        {
            std::cout << "Found Enemy!\n";
            fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
                >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT
                >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
        }
        if (title == "Bullet")
        {
            std::cout << "Found Bullet!\n";
            fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S
                >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
                >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT
                >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }
    // fin >> m_playerConfig.SR >> m_playerConfig.CR;
    //set up window paramaters
    m_window.setFramerateLimit(fps);
    initBackground();
    spawnPlayer();
    auto entity = m_entities.addEntity("text");
    entity->cText = std::make_shared<CText>(m_font, 15, sf::Color(255,255,255));

}

void Game::run()
{
    // TODO: add pause functionality in here
    //       some systems should function while paused (rendering)
    //       some systems shouldn't (movement / input)
    while (m_running)
    {
        if (m_paused)
        {

        sUserInput();
        sRender();

        }
        else
        {
        m_entities.update();

        sEnemySpawner();
        sMovement();
        sCollision();
        sLifespan();
        sUserInput();
        sRender();
        m_currentFrame++;
        }

        // increment the current frame
        // may need to be moved when pause implemented

    }
    m_window.close();
}

void Game::setPaused(bool paused)
{
    // TODO
    m_paused = paused;
}

//respawn the player in the middle of the screen
void Game::spawnPlayer()
{
    // TODO: Finish adding all properties of the player with the correct values from the config

    // We create every entity by calling EntityManager.addEntity(tag)
    // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;

    // Give this entity a Transform so it spawns at (200,200) with velocity (1,1) and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);

    // The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4
    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, 
                                    sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), 
                                    sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

    //Add collision component
    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    // Add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable to be this Entity
    // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
    // TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
    //       the enemy must be spawned completely within the bounds of the window
    //
    
    
    auto entity = m_entities.addEntity("enemy");

    // Give this entity a Transform so it spawns at (200, 200) with velocity (1,1) and angle 0
    //??? you mean random
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;

    int max_x = m_window.getSize().x-m_enemyConfig.SR;
    int min_x = m_enemyConfig.SR;
    int max_y = m_window.getSize().y - m_enemyConfig.SR;
    int min_y = m_enemyConfig.SR;

    int vert = rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1) + m_enemyConfig.VMIN;

    float ex = rand() % (max_x - min_x + 1) + min_x;
    float ey = rand() %( max_y - min_y + 1) + min_y;

    float speed = rand() % ((int)m_enemyConfig.SMAX - (int)m_enemyConfig.SMIN + 1) + m_enemyConfig.SMIN;
    float angle = (180/M_PI)*(rand() % 359);

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(speed*cos(angle), speed*sin(angle)), 0.0f);

    // Create shape from config file with random fill color
    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vert, sf::Color(r, g, b), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

    // Add collision from config
    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

    // record when most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    // TODO: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    int nPoints = e->cShape->circle.getPointCount();
    float angle = 360.0 / nPoints;
    for (int i; i < nPoints; i++)
    {
        auto small = m_entities.addEntity("small enemy");
        small->cTransform = std::make_shared<CTransform>(e->cTransform->pos, e->cTransform->velocity.rotate(i*angle), i*angle);
        small->cShape = std::make_shared<CShape>(m_enemyConfig.SR/2, e->cShape->circle.getPointCount(), e->cShape->circle.getFillColor(), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
        small->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR/2);
        small->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.SI);
    }
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity,const Vec2 & target)
{
    //TODO: implement the spawning of a bullet which travels toward target
    //      - bullet speed is given as a scalar speed
    //      - you must set the velcity by using formula in notes

    auto bullet = m_entities.addEntity("bullet");

    bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, Vec2(0,0) , 0.0f);
    bullet->cTransform->velocity = ((target - entity->cTransform->pos)/entity->cTransform->pos.dist(target)) * m_bulletConfig.S;
    bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    if (m_currentFrame - m_lastSpecialWeaponSpawnTime > 120)
    {
        int nPoints = 1000;
        float angle_rad = (M_PI/180.0)*(360.0 / nPoints);
            for (int i; i < nPoints; i++)
            {
                auto bullet = m_entities.addEntity("bullet");
                bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, Vec2(m_bulletConfig.S * cos(angle_rad * i), m_bulletConfig.S * sin(angle_rad * i)), i * angle_rad);
                bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(0, 0, 0), sf::Color(200, 0, 255), m_bulletConfig.OT);
                bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
                bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L/2.5);
            }
        m_lastSpecialWeaponSpawnTime = m_currentFrame;
    }
}
void Game::spawnSpecialWeapon2(std::shared_ptr<Entity> entity,const Vec2 & target)
{
    if (m_currentFrame - m_lastSpecialWeaponSpawnTime > 120)
    {
        int nPoints = 10;
        float angle_rad = (M_PI/180.0)*(360.0 / nPoints);
            for (int i; i < nPoints; i++)
            {
        auto bullet = m_entities.addEntity("bullet");

        bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, Vec2(m_bulletConfig.S * cos(angle_rad * i), m_bulletConfig.S * sin(angle_rad * i)), i * angle_rad);
        bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR*1.5, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(255,0,255), m_bulletConfig.OT);
        bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L*2);
        bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR*1.5);
        bullet->cGravity = std::make_shared<CGravity>(Vec2(0,0));
            }
        m_lastSpecialWeaponSpawnTime = m_currentFrame;
        }
}

void Game::sMovement()
{
    // TODO: implement all entity movement in this function
    //       you should read the m_player->cInput component to determine if the player is moving

    m_player->cTransform->velocity = {0, 0};
    

    //Damn i just realized i coulda passed in cInput instead of each direction

    //better control logic also in a function
    playerDirection(m_playerConfig.S, m_player->cInput->left, m_player->cInput->right, m_player->cTransform->velocity.x, lastx_input);
    playerDirection(m_playerConfig.S, m_player->cInput->up, m_player->cInput->down, m_player->cTransform->velocity.y, lasty_input);

    //im choosing to normalize the vector outside of playerDIrection()
    //maybe in the future i'll change this and give the function a better name
    if ((m_player->cInput->left || m_player->cInput->right) && (m_player->cInput->up || m_player->cInput->down))
    {
        m_player->cTransform->velocity /= m_player->cTransform->velocity.length();
        m_player->cTransform->velocity *= m_playerConfig.S;
    }

    // Keep player in bounds of window
    // left and right walls
    if ((m_player->cInput->left && m_player->cTransform->pos.x  <=  m_playerConfig.CR) || (m_player->cInput->right && m_player->cTransform->pos.x  >= m_window.getSize().x - m_playerConfig.CR))
    {
        m_player->cTransform->velocity.x = 0;
    }

    // left and right walls
    if ((m_player->cInput->up && m_player->cTransform->pos.y  <=  m_playerConfig.CR) || (m_player->cInput->down && m_player->cTransform->pos.y  >= m_window.getSize().y - m_playerConfig.CR))
    {
        m_player->cTransform->velocity.y = 0;
    }


    // Enemy bounce logic
    for (auto e : m_entities.getEntities("enemy"))
    {
        if ((e->cTransform->pos.x)-(e->cCollision->radius) < 0)
        {
            e->cTransform->velocity.x *= -1;
        }
        if ((e->cTransform->pos.x)+(e->cCollision->radius)> m_window.getSize().x)
        {
            e->cTransform->velocity.x *= -1;
        }
        if ((e->cTransform->pos.y)-(e->cCollision->radius) < 0)
        {
            e->cTransform->velocity.y *= -1;
        }
        if ((e->cTransform->pos.y)+(e->cCollision->radius) > m_window.getSize().y)
        {
            e->cTransform->velocity.y *= -1;
        }
    }
    for (auto e : m_entities.getEntities("small enemy"))
    {
        if ((e->cTransform->pos.x)-(e->cCollision->radius) < 0)
        {
            e->cTransform->velocity.x *= -1;
        }
        if ((e->cTransform->pos.x)+(e->cCollision->radius)> m_window.getSize().x)
        {
            e->cTransform->velocity.x *= -1;
        }
        if ((e->cTransform->pos.y)-(e->cCollision->radius) < 0)
        {
            e->cTransform->velocity.y *= -1;
        }
        if ((e->cTransform->pos.y)+(e->cCollision->radius) > m_window.getSize().y)
        {
            e->cTransform->velocity.y *= -1;
        }
    }

    //Gravity addition
    float G = (3 * pow(10, -3));
    for (auto b : m_entities.getEntities("bullet"))
    {
        if (b->cGravity)
        {
            for (auto e: m_entities.getEntities("enemy"))
            {
                b->cGravity->gravity -= ((b->cTransform->pos-e->cTransform->pos)/e->cTransform->pos.dist(b->cTransform->pos))*G;
            }
            for (auto e: m_entities.getEntities("small enemy"))
            {
                b->cGravity->gravity -= ((b->cTransform->pos-e->cTransform->pos)/e->cTransform->pos.dist(b->cTransform->pos))*0.5*G;
            }
            b->cTransform->velocity += b->cGravity->gravity;
        }
        if ((b->cTransform->pos.x)-(b->cCollision->radius) < 0)
        {
            b->cTransform->velocity.x *= -1;
        }
        if ((b->cTransform->pos.x)+(b->cCollision->radius)> m_window.getSize().x)
        {
            b->cTransform->velocity.x *= -1;
        }
        if ((b->cTransform->pos.y)-(b->cCollision->radius) < 0)
        {
            b->cTransform->velocity.y *= -1;
        }
        if ((b->cTransform->pos.y)+(b->cCollision->radius) > m_window.getSize().y)
        {
            b->cTransform->velocity.y *= -1;
        }
        
    }

    //entity velocty addition
    for (auto e : m_entities.getEntities())
    {
        if(e->cTransform)
           e->cTransform->pos += e->cTransform->velocity;

    }


}

void Game::sLifespan()
{
    // TODO: implement all lifespan functionality
    //
    // for all entities
    //     if entity has no lifespan component, skip it
    //     if entity has > 0 remaining lifespan, subtract 1
    //     if it has lifespan and is alive
    //          scale its alpha channel properly
    //     if it has lifespan and its time is up
    //          destroy the entity
    // check if has lifespan with nullptr or something
    int alpha;
    int r, g, b;//fill colors
    int otr, otg, otb;//outline colors
    //it would be nice to be able to change the alpha value without having to re set all the colors

    for (auto e : m_entities.getEntities())
    {
        
        if ((e->cLifespan))
        {
            if (e->cLifespan->remaining > 0)
            {
                r = e->cShape->circle.getFillColor().r;
                g = e->cShape->circle.getFillColor().g;
                b = e->cShape->circle.getFillColor().b;                r = e->cShape->circle.getFillColor().r;

                otr = e->cShape->circle.getOutlineColor().r;
                otg = e->cShape->circle.getOutlineColor().g;
                otb = e->cShape->circle.getOutlineColor().b;

                alpha = ((float)e->cLifespan->remaining / (float)e->cLifespan->total) * 255.0;
                e->cLifespan->remaining -= 1;
                e->cShape->circle.setFillColor(sf::Color(r, g, b, alpha));
                e->cShape->circle.setOutlineColor(sf::Color(otr, otg, otb, alpha));
            }
            else
            {
                e->destroy();
            }
        }
    }
}

void Game::sCollision()
{
    // TODO: implement all proper collisions between entities
    //       be sure to use the collision radius, NOT the shape radius

    float d; // distance between enemy and bullet
    float d2p; //distance between enemy and player

    for (auto b : m_entities.getEntities("bullet"))
    {
        for (auto e : m_entities.getEntities("enemy"))
        {
            d = (e->cCollision->radius + b->cCollision->radius);
            if (e->cTransform->pos.distsquared(b->cTransform->pos) < d * d)
            {
                e->destroy();
                b->destroy();
                spawnSmallEnemies(e);
            }
        }
        for (auto e : m_entities.getEntities("small enemy"))
        {
            d = (e->cCollision->radius + b->cCollision->radius);
            if (e->cTransform->pos.distsquared(b->cTransform->pos) < d * d)
            {
                e->destroy();
                b->destroy();
            }
        }
    }

    for (auto e : m_entities.getEntities("enemy"))
        {
            d2p = (e->cCollision->radius + m_player->cCollision->radius);
            if (e->cTransform->pos.distsquared(m_player->cTransform->pos) < d2p * d2p)
            {
                m_player->destroy();
                e->destroy();
                spawnSmallEnemies(e);
                spawnPlayer();
                std::cout << "Bro died "<<"💀"<<"\n";
            }
        }
        for (auto e : m_entities.getEntities("small enemy"))
        {
            d2p = (e->cCollision->radius + m_player->cCollision->radius);
            if (e->cTransform->pos.distsquared(m_player->cTransform->pos) < d2p * d2p)
            {
                m_player->destroy();
                e->destroy();
                spawnPlayer();
                std::cout << "Bro died "<<"💀"<<"\n";
            }
        }
}

void Game::sEnemySpawner()
{
    // TODO: code which implements enemy spawning should go here
    //
    //       (use m_currentFrame - m_lastEnemySpawnTime) to determine
    //       how long it has been since last enemy spawned

    if (m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI)
    {
        spawnEnemy();
        m_lastEnemySpawnTime = m_currentFrame;
    }
    m_currentFrame++;
}

void Game::sRender()
{
    // TODO: change the code below to draw ALL of the entities
    //       sample drawing of the player Entity that we have created
    m_window.clear();

    drawBackground();

    for (auto& e : m_entities.getEntities())
    {
        if (e->cShape)
        {
                // set the position of the shape based on the entity's transform->pos
                e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
                
                // set the rotation of the shape based on the entity's transform->angle
                e->cTransform->angle += 2.0f;
                e->cShape->circle.setRotation(e->cTransform->angle);

                
                // draw the entity's sf::CircleShape
                m_window.draw(e->cShape->circle);
        }
        if (e->cText)
        {
            m_window.draw(e->cText->text);
        }
    }
    //for (auto& e : m_entities.getEntities("text"))
   // {
    //    std::cout<<"im in font render\n";
    //    m_window.draw(e->cText->text);
    //}

    m_window.display();
}

void Game::sUserInput()
{
    //TODO: handle user input here
    //      note that you should only be setting the player's input component variables here
    //      you should not implement the player's movement logic here
    //      the movement system will read the variables you set in this function

    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::P:
                    //std::cout << "P key pressed\n";
                    // TODO: toggle paused
                    setPaused(!m_paused);
                    break;
                default: break;
                }
        }
        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    //std::cout << "W key pressed\n";
                    // TODO: set player's input component "up" to true
                    m_player->cInput->up = true;
                    break;
                default: break;
                }
        }

        // this event is triggered when a key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    //std::cout << "W key released\n";
                    // TODO: set player's input component "up" to false
                    m_player->cInput->up = false;
                    break;
                default: break;
                }
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::S:
                    //std::cout << "S key pressed\n";
                    // TODO: set player's input component "down" to true
                    m_player->cInput->down = true;
                    break;
                default: break;
                }
        }

        // this event is triggered when a key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::S:
                    //std::cout << "S key released\n";
                    // TODO: set player's input component "down" to false
                    m_player->cInput->down = false;
                    break;
                default: break;
                }
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::A:
                    //std::cout << "A key pressed\n";
                    // TODO: set player's input component "left" to true
                    m_player->cInput->left = true;
                    break;
                default: break;
                }
        }

        // this event is triggered when a key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::A:
                    //std::cout << "A key released\n";
                    // TODO: set player's input component "left" to false
                    m_player->cInput->left = false;
                    break;
                default: break;
                }
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::D:
                    //std::cout << "D key pressed\n";
                    // TODO: set player's input component "right" to true
                    m_player->cInput->right = true;
                    break;
                default: break;
                }
        }

        // this event is triggered when a key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::D:
                    //std::cout << "D key released\n";
                    // TODO: set player's input component "right" to false
                    m_player->cInput->right = false;
                    break;
                default: break;
                }
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Num1:
                    //num1 pressed
                    m_specialWeaponType = 0;
                    break;
                default: break;
                }
        }
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Num2:
                    //num2 pressed
                    m_specialWeaponType = 1;
                    break;
                default: break;
                }
        }

        if (!m_paused) // inputs to disable when paused
        {              // this is the best way i've found to maintain directional input
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                if (m_specialWeaponType==0)
                {
                //spawn cool bullet
                    spawnSpecialWeapon(m_player);
                }
                else if (m_specialWeaponType==1)
                {
                    spawnSpecialWeapon2(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
                }
            }
        }
        }
    }
}

void Game::playerDirection(float speed, const bool & dir1, const bool & dir2, float & dir_vel, bool & last_input)
{
    //This implements direction overwite when two opposing keys are pressed
    if (dir1 && !dir2)
    {
        dir_vel = -speed;
        last_input = false;
    }
    else if (dir2 && !dir1)
    {
        dir_vel = speed;
        last_input = true;
    }
    else if (dir1 && dir2)
    {
        if (last_input)
            dir_vel = -speed;
        else
            dir_vel = speed;
    }
}

void Game::initBackground()
{
    //im not sure if drawing a background outside of the entity manager goes against ECS principle.
    //i dont feel like adding a rectangle component
    int pixel_interval = 20;
    sf::RectangleShape background(sf::Vector2f(pixel_interval, pixel_interval));

    for (int i = 0; i < (m_window.getSize().x);i+=2*pixel_interval)
    {
        for (int j = 0; j < (m_window.getSize().y);j+=pixel_interval)
        {
            if (((j/pixel_interval))%2 == 0)
            {
                background.setFillColor(sf::Color(5, 5, 5));
                background.setPosition(sf::Vector2f(i, j));
                m_backgroundVec.push_back(background);
                background.setFillColor(sf::Color(10, 10, 10));
                background.setPosition(sf::Vector2f(i+pixel_interval, j));
                m_backgroundVec.push_back(background);
            }
            else
            {
                background.setFillColor(sf::Color(10, 10, 10));
                background.setPosition(sf::Vector2f(i, j));
                m_backgroundVec.push_back(background);
                background.setFillColor(sf::Color(15, 15, 15));
                background.setPosition(sf::Vector2f(i+pixel_interval, j));
                m_backgroundVec.push_back(background);
            }
        }
    }
}

void Game::drawBackground()
{
    for (auto rec : m_backgroundVec)
    {
        m_window.draw(rec);
    }
}