#include <SFML/Graphics.hpp>
#include<iostream>
#include <SFML/Audio.hpp>


//function prototypes
void pacAnimate(sf::Sprite& pacMan,bool& left,bool& right,bool& up,bool& down,int& track,bool& isDead);
void pacCollide (sf::FloatRect& bounds,sf::Vector2f& position,sf::Image& collision, sf::Vector2f& lastPosition,
                 sf::Sprite& pacMan,bool& canMove,bool& left,bool& right,bool& up,bool& down,int& track);
void pacMove(sf::Sprite& pacMan,bool& canMove,bool& left,bool& right,
             bool& up,bool& down,sf::Image& collision,sf::FloatRect& bounds,sf::Vector2f& position);
void pacDir(sf::Sprite& pacMan,bool& left,bool& right,bool& up,bool& down,const float& speed);
void pacTunnel(sf::Sprite& pacMan);
void food(sf::Image& foodMap,std::vector<sf::RectangleShape>& pacFood,int& countt);
void foodDel(sf::Sprite& pacMan,std::vector<sf::RectangleShape>& pacFood,sf::Sound& eat,sf::Sound& eat2,int& foodNum,bool& played,int& foodEaten,int& scoreNum);
void ghostDir(std::vector<sf::Sprite>& ghostVec,sf::Image& collision,sf::Sprite& pacMan,sf::FloatRect ghBounds[4],sf::Vector2f ghPosition[4],
               sf::FloatRect& bounds,sf::Vector2f& position,bool ghLeft[4],bool ghRight[4],bool ghUp[4],bool ghDown[4],int random[4],int cnt[4]);
void ghostMove(std::vector<sf::Sprite>& ghostVec,bool ghLeft[4],bool ghRight[4],bool ghUp[4],bool ghDown[4],int random[4],int& ghTrack,float& ghSpeed);
int randDir(int random[4],int i,int cnt[4]);
void ghostTunnel(std::vector<sf::Sprite>& ghostVec);
void ghostAnimate(std::vector<sf::Sprite>& ghostVec,bool ghLeft[4],bool ghRight[4],bool ghUp[4],bool ghDown[4],int& ghTrack,int random[4]);
void ghostRelease(std::vector<sf::Sprite>& ghostVec,int& releaseNum);
void sirenCondition(int foodEaten,sf::Music siren[3],bool sirenPlayed[3]);
void sirenMusic(sf::Music siren[3],bool sirenPlayed[3]);
void pacDie(std::vector<sf::Sprite>& ghostVec,sf::Sprite& pacMan,sf::FloatRect& bounds,sf::FloatRect ghBounds[4],int& dieNum,float& speed,float& ghSpeed,bool& isDead,sf::Sound& die);
void deathAnimate(sf::Sprite& pacMan,int& dieNum,bool& isDead,bool& canMove,bool& left,bool& right,bool& up,bool& down,float& speed,
                  float& ghSpeed,sf::Sound& die2,sf::Sound& die,int& releaseNum,int& lifeNum,std::vector<sf::Sprite>& lives,sf::Text& over);
void gameStart(sf::Text& ready,int& foodNum);
void loseLife(std::vector<sf::Sprite>& lives,int& lifeNum,sf::Text& over,int& dieNum);
void reset(sf::Sprite& pacMan,int& dieNum,bool& isDead,bool& canMove,bool& left,bool& right,bool& up,bool& down,float& speed,
           float& ghSpeed,sf::Sound& die2,sf::Sound& die,int& releaseNum,int& lifeNum,std::vector<sf::Sprite>& lives,sf::Text& over);
void gameOver(sf::RenderWindow& app,sf::Text& over,int& lifeNum,int& dieNum);
void won(int& foodEaten,sf::RenderWindow& app,sf::Text& win,std::vector<sf::RectangleShape>& pacFood);
int main(){
    //variables
    //(gh=ghost)
    bool path{false},canMove{true},left{false},right{false},up{false},down{false},ghLeft[4]{false},ghRight[4]{false},ghUp[4]{false},ghDown[4]{false},played{false},sirenPlayed[3]{false},isDead{false};
    float speed{0.13},ghSpeed{0.16};
    int track{0},ghTrack{0},countt{0},random[4]{rand()%4+1},cnt[4]{0},foodNum{0},foodEaten{0},dieNum{0},releaseNum{0},scoreNum{0},lifeNum{0};
    srand(time(NULL));

    sf::RenderWindow app(sf::VideoMode(550,600), "Pacman");

    sf::Texture texture;
    texture.loadFromFile("PacSprites.png");

    //sprites
    sf::Sprite fMaze;
    fMaze.setTexture(texture);
    fMaze.setTextureRect(sf::IntRect(169,1,167,191));
    fMaze.setScale(3,3);
    fMaze.setPosition(3,0);

    sf::Sprite pacMan;
    pacMan.setTexture(texture);
    pacMan.setTextureRect(sf::IntRect(337,1,14,14));
    pacMan.setScale(2,2);
    pacMan.setOrigin(7,7);
    pacMan.setPosition(252,419);

     //ghost vector
    std::vector<sf::Sprite>ghostVec;
    for(int i=0;i<4;i++){
        sf::Sprite ghost;
        ghost.setTexture(texture);
        ghost.setTextureRect(sf::IntRect(0,0,0,0));
        ghost.setScale(1.85f,1.85f);
        ghost.setOrigin(7,7);
        ghost.setPosition(i*26+225,255);
        ghostVec.push_back(ghost);
    }
    ghostVec[3].setPosition(260,207);

    std::vector<sf::Sprite>lives;
    for(int i=0;i<3;i++){
    sf::Sprite life;
    life.setTexture(texture);
    life.setTextureRect(sf::IntRect(402,1,14,14));
    life.setScale(1.5,1.5);
    life.setPosition(i*26+415,573);
    lives.push_back(life);
    }
    //images
    //collision image has all walls as black pixels, pathways as white
    sf::Image collision;
    collision.loadFromFile("Collision.png");
    //food image shows where to place food on the screen
    sf::Image foodMap;
    foodMap.loadFromFile("foodMap.png");

    sf::Texture texture2;//delete later
    texture2.loadFromImage(collision);
    sf::Sprite sprite;
    sprite.setTexture(texture2);

    //audio
    sf::SoundBuffer startBuffer;
    startBuffer.loadFromFile("game_start.wav");
    sf::Sound start;
    start.setBuffer(startBuffer);
    start.play();

    sf::SoundBuffer foodBuffer[2];
    foodBuffer[0].loadFromFile("munch.wav");
    sf::Sound eat;
    eat.setBuffer(foodBuffer[0]);
    foodBuffer[1].loadFromFile("munch_2.wav");
    sf::Sound eat2;
    eat2.setBuffer(foodBuffer[1]);

    sf::SoundBuffer dieBuffer[2];
    dieBuffer[0].loadFromFile("death_1.wav");
    sf::Sound die;
    die.setBuffer(dieBuffer[0]);
    dieBuffer[1].loadFromFile("death_2.wav");
    sf::Sound die2;
    die2.setBuffer(dieBuffer[1]);

    sf::Music siren[3];
    siren[0].openFromFile("siren_1.wav");
    siren[1].openFromFile("siren_2.wav");
    siren[2].openFromFile("siren_3.wav");
    for (int i=0;i<3;i++){
    siren[i].play();
    siren[i].setLoop(true);
    siren[i].setVolume(0);
    }
    //text
    sf::Font pacFont;
    pacFont.loadFromFile("pacFont.ttf");

    sf::Text ready;
    ready.setFont(pacFont);
    ready.setCharacterSize(20);
    ready.setFillColor(sf::Color::Yellow);
    ready.setString("READY!");
    ready.setPosition(212,310);

    sf::Text score;
    score.setFont(pacFont);
    score.setPosition(7,564);

    sf::Text over;
    over.setFont(pacFont);
    over.setString("GAME       OVER");
    over.setCharacterSize(20);
    over.setFillColor(sf::Color::Red);
    over.setPosition(-600,310);

     sf::Text win;
    win.setFont(pacFont);
    win.setCharacterSize(20);
    win.setFillColor(sf::Color::Green);
    win.setString("Winner!");
    win.setPosition(212,-310);

    //food vector
    std::vector<sf::RectangleShape>pacFood;
     for (int i=0;i<174;i++) {
    sf::RectangleShape square(sf::Vector2f(6,6));
    square.setFillColor(sf::Color(250,185,176));
    pacFood.push_back(square);
     }
     //places food where ever a yellow pixel is on food image
    for(int y=0;y<foodMap.getSize().y;y++){
        for(int x=0;x<foodMap.getSize().x;x++){
            if (foodMap.getPixel(x,y)==sf::Color::Yellow) {
                 pacFood[countt].setPosition(x,y);
                countt++;
            }
        }
    }
    //shortened code for finding bounds,position
    sf::FloatRect ghBounds[4];
    sf::Vector2f ghPosition[4];

    sf::FloatRect bounds;
    sf::Vector2f position;
    sf::Vector2f lastPosition = pacMan.getPosition();

    while (app.isOpen()){
        releaseNum++;
        track++;
        ghTrack++;
        dieNum++;
        for(int i=0;i<4;i++){
        cnt[i]++;
        }
        sf::Event event;
        while (app.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                app.close();
          }
        //shortened code for bounds,position
        for(int i=0;i<4;i++){
            ghBounds[i]=ghostVec[i].getGlobalBounds();
            ghPosition[i]=ghostVec[i].getPosition();
        }
        bounds = pacMan.getGlobalBounds();
        position = pacMan.getPosition();

        //function calls
        pacCollide(bounds,position,collision,lastPosition,pacMan,canMove,left,right,up,down,track);
        pacMove(pacMan,canMove,left,right,up,down,collision,bounds,position);
        pacDir(pacMan,left,right,up,down,speed);
        pacAnimate(pacMan,left,right,up,down,track,isDead);
        pacTunnel(pacMan);
        foodDel(pacMan,pacFood,eat,eat2,foodNum,played,foodEaten,scoreNum);
        ghostDir(ghostVec,collision,pacMan,ghBounds,ghPosition,bounds,position,ghLeft,ghRight,ghUp,ghDown,random,cnt);
        ghostMove(ghostVec,ghLeft,ghRight,ghUp,ghDown,random,ghTrack,ghSpeed);
        ghostTunnel(ghostVec);
        ghostRelease(ghostVec,releaseNum);
        sirenCondition(foodEaten,siren,sirenPlayed);
        pacDie(ghostVec,pacMan,bounds,ghBounds,dieNum,speed,ghSpeed,isDead,die);
        deathAnimate(pacMan,dieNum,isDead,canMove,left,right,up,down,speed,ghSpeed,die2,die,releaseNum,lifeNum,lives,over);
        gameStart(ready,foodNum);
        gameOver(app,over,lifeNum,dieNum);
        won(foodEaten,app,win,pacFood);
        score.setString(std::to_string(scoreNum));

        app.clear();
        app.draw(fMaze);
        app.draw(ready);
        app.draw(score);
        app.draw(win);
        app.draw(over);
        for(int i=0;i<lives.size();i++){
        app.draw(lives[i]);
        }
        for(int i=0;i<pacFood.size();i++){
        app.draw(pacFood[i]);
        }
        app.draw(pacMan);
        for(int i=0;i<ghostVec.size();i++){
        app.draw(ghostVec[i]);
        }
        app.display();
    }
    return 0;
}
//function definitions
void pacAnimate(sf::Sprite& pacMan,bool& left,bool& right,bool& up,bool& down,int& track,bool& isDead){
    //animation for pacman sprite
    if(isDead==false){
    if(left==true){
        if (track==1){
            pacMan.setTextureRect(sf::IntRect(337,17,14,14));
        } else if (track==70){
            pacMan.setTextureRect(sf::IntRect(402,1,14,14));
        } else if(track==140) {
            pacMan.setTextureRect(sf::IntRect(418,1,14,14));
        } else if(track==210) {
            pacMan.setTextureRect(sf::IntRect(402,1,14,14));
        }
    }
  if(right==true){
        if (track==1){
            pacMan.setTextureRect(sf::IntRect(337,17,14,14));
        } else if (track==70){
            pacMan.setTextureRect(sf::IntRect(337,1,14,14));
        } else if(track==140) {
            pacMan.setTextureRect(sf::IntRect(353,1,14,14));
        } else if(track==210) {
            pacMan.setTextureRect(sf::IntRect(337,1,14,14));
        }
    }
 if(up==true){
        if (track==1){
            pacMan.setTextureRect(sf::IntRect(337,17,14,14));
        } else if (track==70){
            pacMan.setTextureRect(sf::IntRect(369,1,14,14));
        } else if(track==140) {
            pacMan.setTextureRect(sf::IntRect(385,1,14,14));
        } else if(track==210) {
            pacMan.setTextureRect(sf::IntRect(369,1,14,14));
        }
    }
 if(down==true){
        if (track==1){
            pacMan.setTextureRect(sf::IntRect(337,17,14,14));
        } else if (track==70){
            pacMan.setTextureRect(sf::IntRect(433,1,14,14));
        } else if(track==140) {
            pacMan.setTextureRect(sf::IntRect(449,1,14,14));
        } else if(track==210) {
            pacMan.setTextureRect(sf::IntRect(433,1,14,14));
        }
    }
    }
    if(track>280){
        track=0;
    }
}
void pacCollide (sf::FloatRect& bounds,sf::Vector2f& position,sf::Image& collision, sf::Vector2f& lastPosition,sf::Sprite& pacMan,bool& canMove,bool& left,bool& right,bool& up,bool& down,int& track){
  //Collision Detection - if pacman is in contact with a wall, stop movement in current direction
      if (collision.getPixel(position.x-bounds.width/2,position.y-bounds.height/2)==sf::Color::Black|| // Top left corner
    collision.getPixel(position.x+bounds.width/2,position.y-bounds.height/2)==sf::Color::Black|| // Top right corner
    collision.getPixel(position.x-bounds.width/2,position.y+bounds.height/2)==sf::Color::Black|| // Bottom left corner
    collision.getPixel(position.x+bounds.width/2,position.y+bounds.height/2)==sf::Color::Black){ // Bottom right corner
        pacMan.setPosition(lastPosition);
        track=210;
        canMove=false;
        left=false;
        right=false;
        up=false;
        down=false;
    } else {lastPosition = position;
    }
}
void pacMove(sf::Sprite& pacMan,bool& canMove,bool& left,bool& right,bool& up,bool& down,sf::Image& collision,sf::FloatRect& bounds,sf::Vector2f& position){
    //if pixels infront of pacman in desired direction are white(pathway), lets it move in that direction
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        if(canMove==false||
           (collision.getPixel(position.x-bounds.width,position.y-bounds.height/2)==sf::Color::White&&
            collision.getPixel(position.x-bounds.width,position.y+bounds.height/2)==sf::Color::White&&
            collision.getPixel(position.x-bounds.width,position.y)==sf::Color::White)){
            canMove=true;
            left=true;
            right=false;
            down=false;
            up=false;
        }
        }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            if(canMove==false||
           (collision.getPixel(position.x+bounds.width,position.y-bounds.height/2)==sf::Color::White&&
            collision.getPixel(position.x+bounds.width,position.y+bounds.height/2)==sf::Color::White&&
            collision.getPixel(position.x+bounds.width,position.y)==sf::Color::White)){
                canMove=true;
                right=true;
                left=false;
                up=false;
                down=false;
            }
        }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            if(canMove==false||
           (collision.getPixel(position.x-bounds.width/2,position.y-bounds.height)==sf::Color::White&&
            collision.getPixel(position.x+bounds.width/2,position.y-bounds.height)==sf::Color::White&&
            collision.getPixel(position.x,position.y-bounds.height)==sf::Color::White)){
                canMove=true;
                up=true;
                down=false;
                left=false;
                right=false;
            }
        }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            if(canMove==false||
           (collision.getPixel(position.x-bounds.width/2,position.y+bounds.height)==sf::Color::White&&
            collision.getPixel(position.x+bounds.width/2,position.y+bounds.height)==sf::Color::White&&
            collision.getPixel(position.x,position.y+bounds.height)==sf::Color::White)){
                canMove=true;
                down=true;
                up=false;
                left=false;
                right=false;
        }
        }
}
void pacDir(sf::Sprite& pacMan,bool& left,bool& right,bool& up,bool& down,const float& speed){
    //moves pacman in desired direction
    if(left==true){
        pacMan.move(-speed,0);
    }
    if(right==true){
        pacMan.move(speed,0);
    }
    if(up==true){
        pacMan.move(0,-speed);
    }
    if(down==true){
        pacMan.move(0,speed);
    }
}
void pacTunnel(sf::Sprite&pacMan){
    //if pacman goes through the tunnel on either side of maze, teleport to other tunnel
if(pacMan.getPosition().x<-3){
    pacMan.setPosition(501,pacMan.getPosition().y);
}
if(pacMan.getPosition().x>508){
    pacMan.setPosition(0,pacMan.getPosition().y);
}
}

void foodDel(sf::Sprite& pacMan,std::vector<sf::RectangleShape>& pacFood,sf::Sound& eat,sf::Sound& eat2,int& foodNum,bool& played,int& foodEaten,int& scoreNum){
    //if pacman comes in contact (eats) a peice of food, move it off the screen, play sound
for(int i=0;i<pacFood.size();i++){
    if(pacMan.getGlobalBounds().intersects(pacFood[i].getGlobalBounds())){
            pacFood[i].move(0,-600);
            foodNum++;
            foodEaten++;
            scoreNum +=10;
        break;
    }
    if(foodNum==1&&!played){
        eat.play();
        played=true;
    } else if(foodNum==2){
        foodNum=0;
        played=false;
        eat2.play();
    }
}
}
int randDir(int random[4],int i,int cnt[4]){
    //generates a random direction for a ghost to move when called
    static int tempRand[4]={0};
    if (cnt[i]>550){
    do{
    random[i]=rand()%4+1;
    cnt[i]=0;
    } while (random[i]==tempRand[i]||
        (tempRand[i]==1&&random[i]==2)||(tempRand[i]==2&&random[i]==1)||
        (tempRand[i]==3&&random[i]==4)||(tempRand[i]==4&&random[i]==3));
                tempRand[i]=random[i];
        return random[i];
    }
}
void ghostDir(std::vector<sf::Sprite>& ghostVec,sf::Image& collision,sf::Sprite& pacMan,sf::FloatRect ghBounds[4],sf::Vector2f ghPosition[4],
               sf::FloatRect& bounds,sf::Vector2f& position,bool ghLeft[4],bool ghRight[4],bool ghUp[4],bool ghDown[4],int random[4],int cnt[4]){
    //checks if theres a possible pathway to move to and if there is, generate random number
    static float devide{1.4};
    for(int i=0;i<ghostVec.size();i++){
                if(ghRight[i]==false&&collision.getPixel(ghPosition[i].x-ghBounds[i].width/devide,ghPosition[i].y-ghBounds[i].height/2)==sf::Color::White&&
            collision.getPixel(ghPosition[i].x-ghBounds[i].width/devide,ghPosition[i].y+ghBounds[i].height/2)==sf::Color::White&&
            collision.getPixel(ghPosition[i].x-ghBounds[i].width/devide,ghPosition[i].y)==sf::Color::White){
                ghLeft[i]=true;
                if(ghLeft[i]==true&&(ghDown[i]==true||ghUp[i]==true)){
                    randDir(random,i,cnt);
                }
        }else {ghLeft[i]=false;}

                if(ghLeft[i]==false&&collision.getPixel(ghPosition[i].x+ghBounds[i].width/devide,ghPosition[i].y-ghBounds[i].height/2)==sf::Color::White&&
            collision.getPixel(ghPosition[i].x+ghBounds[i].width/devide,ghPosition[i].y+ghBounds[i].height/2)==sf::Color::White&&
            collision.getPixel(ghPosition[i].x+ghBounds[i].width/devide,ghPosition[i].y)==sf::Color::White){
                ghRight[i]=true;
                if(ghRight[i]==true&&(ghUp[i]==true||ghDown[i]==true)){
                    randDir(random,i,cnt);
                }
            }else {ghRight[i]=false;}

                if(ghDown[i]==false&&collision.getPixel(ghPosition[i].x-ghBounds[i].width/2,ghPosition[i].y-ghBounds[i].height/devide)==sf::Color::White&&
            collision.getPixel(ghPosition[i].x+ghBounds[i].width/2,ghPosition[i].y-ghBounds[i].height/devide)==sf::Color::White&&
            collision.getPixel(ghPosition[i].x,ghPosition[i].y-ghBounds[i].height/devide)==sf::Color::White){
                ghUp[i]=true;
                 if(ghUp[i]==true&&(ghRight[i]==true||ghLeft[i]==true)){
                    randDir(random,i,cnt);
                }
            }else {ghUp[i]=false;}

                if(ghUp[i]==false&&collision.getPixel(ghPosition[i].x-ghBounds[i].width/2,ghPosition[i].y+ghBounds[i].height/devide)==sf::Color::White&&
            collision.getPixel(ghPosition[i].x+ghBounds[i].width/2,ghPosition[i].y+ghBounds[i].height/devide)==sf::Color::White&&
            collision.getPixel(ghPosition[i].x,ghPosition[i].y+ghBounds[i].height/devide)==sf::Color::White){
                ghDown[i]=true;
                if(ghDown[i]==true&&(ghRight[i]==true||ghLeft[i]==true)){
                    randDir(random,i,cnt);
                }
            }else {ghDown[i]=false;}
}
}
void ghostMove(std::vector<sf::Sprite>& ghostVec,bool ghLeft[4],bool ghRight[4],bool ghUp[4],bool ghDown[4],int random[4],int& ghTrack,float& ghSpeed){
    //if potential direction and random number match or potential direction is the only possible direction, move in that direction
    for(int i=0;i<ghostVec.size();i++){
    if((ghLeft[i]==true&&random[i]==1)||(ghLeft[i]==true&&(!ghRight[i]&&!ghUp[i]&&!ghDown[i]))){//left
        ghostVec[i].move(-ghSpeed,0);
        ghostAnimate(ghostVec,ghLeft,ghRight,ghUp,ghDown,ghTrack,random);
    }
    if((ghRight[i]==true&&random[i]==2)||(ghRight[i]==true&&(!ghLeft[i]&&!ghUp[i]&&!ghDown[i]))){//right
        ghostVec[i].move(ghSpeed,0);
        ghostAnimate(ghostVec,ghLeft,ghRight,ghUp,ghDown,ghTrack,random);
    }
    if((ghUp[i]==true&&random[i]==3)||(ghUp[i]==true&&(!ghRight[i]&&!ghLeft[i]&&!ghDown[i]))){//up
        ghostVec[i].move(0,-ghSpeed);
        ghostAnimate(ghostVec,ghLeft,ghRight,ghUp,ghDown,ghTrack,random);
    }
    if((ghDown[i]==true&&random[i]==4)||(ghDown[i]==true&&(!ghRight[i]&&!ghUp[i]&&!ghRight[i]))){//down
        ghostVec[i].move(0,ghSpeed);
        ghostAnimate(ghostVec,ghLeft,ghRight,ghUp,ghDown,ghTrack,random);
    }
}
}
void ghostTunnel(std::vector<sf::Sprite>& ghostVec){
     //if either of the ghosts go through the tunnel on either side of maze, teleport to other tunnel

    for(int i=0;i<ghostVec.size();i++){
if(ghostVec[i].getPosition().x<-3){
    ghostVec[i].setPosition(501,ghostVec[i].getPosition().y);
}
if(ghostVec[i].getPosition().x>508){
    ghostVec[i].setPosition(0,ghostVec[i].getPosition().y);
}
}
}
void ghostAnimate(std::vector<sf::Sprite>& ghostVec,bool ghLeft[4],bool ghRight[4],bool ghUp[4],bool ghDown[4],int& ghTrack,int random[4]){
 //animation for ghosts
    for(int i=0;i<ghostVec.size();i++){
 if(ghLeft[i]==true&&random[i]==1){
        if (ghTrack==1){
            ghostVec[0].setTextureRect(sf::IntRect(369,113,14,14));
            ghostVec[1].setTextureRect(sf::IntRect(369,97,14,14));
            ghostVec[2].setTextureRect(sf::IntRect(369,129,14,14));
            ghostVec[3].setTextureRect(sf::IntRect(369,81,14,14));
        } else if (ghTrack==140){
            ghostVec[0].setTextureRect(sf::IntRect(385,113,14,14));
            ghostVec[1].setTextureRect(sf::IntRect(385,97,14,14));
            ghostVec[2].setTextureRect(sf::IntRect(385,129,14,14));
            ghostVec[3].setTextureRect(sf::IntRect(385,81,14,14));
        }
    }
  if(ghRight[i]==true&&random[i]==2){
        if (ghTrack==1){
            ghostVec[0].setTextureRect(sf::IntRect(337,113,14,14));
            ghostVec[1].setTextureRect(sf::IntRect(337,97,14,14));
            ghostVec[2].setTextureRect(sf::IntRect(337,129,14,14));
            ghostVec[3].setTextureRect(sf::IntRect(337,81,14,14));
        } else if (ghTrack==140){
            ghostVec[0].setTextureRect(sf::IntRect(353,113,14,14));
            ghostVec[1].setTextureRect(sf::IntRect(353,97,14,14));
            ghostVec[2].setTextureRect(sf::IntRect(353,129,14,14));
            ghostVec[3].setTextureRect(sf::IntRect(353,81,14,14));
        }
    }
 if(ghUp[i]==true&&random[i]==3){
        if (ghTrack==1){
            ghostVec[0].setTextureRect(sf::IntRect(401,113,14,14));
            ghostVec[1].setTextureRect(sf::IntRect(401,97,14,14));
            ghostVec[2].setTextureRect(sf::IntRect(401,129,14,14));
            ghostVec[3].setTextureRect(sf::IntRect(401,81,14,14));
        } else if (ghTrack==140){
            ghostVec[0].setTextureRect(sf::IntRect(417,113,14,14));
            ghostVec[1].setTextureRect(sf::IntRect(417,97,14,14));
            ghostVec[2].setTextureRect(sf::IntRect(417,129,14,14));
            ghostVec[3].setTextureRect(sf::IntRect(417,81,14,14));
        }
    }
 if(ghDown[i]==true&&random[i]==4){
        if (ghTrack==1){
            ghostVec[0].setTextureRect(sf::IntRect(433,113,14,14));
            ghostVec[1].setTextureRect(sf::IntRect(433,97,14,14));
            ghostVec[2].setTextureRect(sf::IntRect(433,129,14,14));
            ghostVec[3].setTextureRect(sf::IntRect(433,81,14,14));
        } else if (ghTrack==140){
            ghostVec[0].setTextureRect(sf::IntRect(449,113,14,14));
            ghostVec[1].setTextureRect(sf::IntRect(449,97,14,14));
            ghostVec[2].setTextureRect(sf::IntRect(449,129,14,14));
            ghostVec[3].setTextureRect(sf::IntRect(449,81,14,14));
        }
    }
    if(ghTrack>280){
        ghTrack=0;
    }
}
}
void ghostRelease(std::vector<sf::Sprite>& ghostVec,int& releaseNum){
//releases each ghost at different times
    if(releaseNum==4000){
        ghostVec[0].setPosition(260,207);
    } else if(releaseNum==10000){
        ghostVec[1].setPosition(260,207);
    } else if(releaseNum==17000){
        ghostVec[2].setPosition(260,207);
    }
}
void sirenCondition(int foodEaten,sf::Music siren[3],bool sirenPlayed[3]){
//sirens speed up over time
    if(foodEaten==2&&sirenPlayed[0]==false){
        sirenPlayed[0]=true;
        sirenMusic(siren,sirenPlayed);
    }
    if (foodEaten==50&&sirenPlayed[1]==false){
        sirenPlayed[1]=true;
        sirenMusic(siren,sirenPlayed);
        }
    if(foodEaten==130&&sirenPlayed[2]==false){
        sirenPlayed[2]=true;
        sirenMusic(siren,sirenPlayed);
        }
}
void sirenMusic(sf::Music siren[3],bool sirenPlayed[3]){
//switches which siren speed is being played
    if(sirenPlayed[0]==true){
        siren[0].setVolume(100);
    }
    if(sirenPlayed[1]==true){
        siren[0].stop();
        siren[1].setVolume(100);
    }
    if(sirenPlayed[2]==true){
        siren[1].stop();
       siren[2].setVolume(100);
    }

}
void pacDie(std::vector<sf::Sprite>& ghostVec,sf::Sprite& pacMan,sf::FloatRect& bounds,sf::FloatRect ghBounds[4],int& dieNum,float& speed,float& ghSpeed,bool& isDead,sf::Sound& die){
//puts pacman and ghosts back to starting position
    for(int i=0;i<ghostVec.size();i++){
    if(ghBounds[i].intersects(bounds)){
        die.play();
        speed=0;
        ghSpeed=0;
        ghostVec[0].setPosition(225,255);
        ghostVec[1].setPosition(251,255);
        ghostVec[2].setPosition(277,255);
        ghostVec[3].setPosition(260,207);
        dieNum=0;
        isDead=true;
    }

}
}
void deathAnimate(sf::Sprite& pacMan,int& dieNum,bool& isDead,bool& canMove,bool& left,bool& right,bool& up,bool& down,float& speed,float& ghSpeed,sf::Sound& die2,sf::Sound& die,int& releaseNum,int& lifeNum,std::vector<sf::Sprite>& lives,sf::Text& over){
//animation for pacmans death
    if(isDead==true){
    if(dieNum==40){
        pacMan.setTextureRect(sf::IntRect(353,17,14,14));
    }
    if(dieNum==80){
        pacMan.setTextureRect(sf::IntRect(369,17,14,14));
    }
    if(dieNum==120){
        pacMan.setTextureRect(sf::IntRect(385,17,14,14));
    }
    if(dieNum==160){
        pacMan.setTextureRect(sf::IntRect(401,17,14,14));
    }
    if(dieNum==200){
        pacMan.setTextureRect(sf::IntRect(417,17,14,14));
    }
    if(dieNum==240){
        pacMan.setTextureRect(sf::IntRect(433,17,14,14));
    }
    if(dieNum==280){
        pacMan.setTextureRect(sf::IntRect(449,17,14,14));
    }
    if(dieNum==320){
        pacMan.setTextureRect(sf::IntRect(465,17,14,14));
    }
    if(dieNum==360){
        pacMan.setTextureRect(sf::IntRect(337,17,14,14));
        die2.play();
    }
    if(dieNum==400){
        pacMan.setTextureRect(sf::IntRect(481,17,14,14));
    }
    if(dieNum==440){
        pacMan.setTextureRect(sf::IntRect(497,17,14,14));
    }
    if(dieNum==520){
        reset(pacMan,dieNum,isDead,canMove,left,right,up,down,speed,ghSpeed,die2,die,releaseNum,lifeNum,lives,over);
    }
    }

}
void gameStart(sf::Text& ready,int& foodNum){
    //removes ready text from screen after pacman starts moving
    if(foodNum==2){
        ready.setPosition(0,600);
    }
}
void loseLife(std::vector<sf::Sprite>& lives,int& lifeNum,sf::Text& over){
    //removes a life from the screen each time pacman dies. 4 deaths display the over message and close the program
    if(lifeNum>0){
    lives[lifeNum-1].setPosition(0,-600);
    }
}
void reset(sf::Sprite& pacMan,int& dieNum,bool& isDead,bool& canMove,bool& left,bool& right,bool& up,bool& down,float& speed,float& ghSpeed,sf::Sound& die2,sf::Sound& die,int& releaseNum,int& lifeNum,std::vector<sf::Sprite>& lives,sf::Text& over){
//resets everthing back to normal after each death
    isDead=false;
    canMove=true;
    left=false;
    right=false;
    up=false;
    down=false;
    speed=0.13;
    ghSpeed=0.16;
    pacMan.setTextureRect(sf::IntRect(337,1,14,14));
    pacMan.setPosition(252,419);
    die2.play();
    die.stop();
    releaseNum=0;
    lifeNum++;
    loseLife(lives,lifeNum,over);
}

void gameOver(sf::RenderWindow& app,sf::Text& over,int& lifeNum,int& dieNum){
 if(lifeNum==4){
        over.setPosition(167,310);
        app.setFramerateLimit(1);
        if(dieNum==530){
            app.close();
        }
    }
}
void won(int& foodEaten,sf::RenderWindow& app,sf::Text& win,std::vector<sf::RectangleShape>& pacFood){
    if(foodEaten==pacFood.size()){
        win.setPosition(208,310);
        app.setFramerateLimit(1);
    }
}

