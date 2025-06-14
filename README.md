# Minecraft (160 pont)

## Rövid feladatleírás

A feladatban a Minecraft egy egyszerűsített változatát kell létrehozni. A játéktér 1\*1\*1 m-es kocka alakú blokkokból épül fel. A blokkok a világ koordinátatengelyeivel párhuzamos szabályos kockarácson helyezkednek el. A különböző típusú blokkok ebben a feladatban mindig kitöltik a teljes kockát (nem feladat pl. ládák és egyéb, speciális modellel rendelkező blokkok leprogramozása). A textúrákat textúraatlaszból vesszük. A játék fontos része, hogy a környezet procedurálisan generált, a feladatban ennek is megoldandó egy egyszerűsített változata. A generált világ ezután szabadon módosítható: a blokkok törhetőek, lerakhatók új blokkok.

## 1 Kötelezően megoldandó rész (45 pont)

### 1.1 Megjelenítés (15 pont)

A gyors megjelenítés érdekében nem egyesével rajzoljuk ki a kockákat. Egyszerre sok blokkot rajzolunk, és csak azokat a blokk-oldalakat rajzoljuk ki, amik megjelenhetnek, azaz levegővel érintkeznek az adott oldalon. Ezeket rakjuk a VBO-ba.  
Mivel módosításkor újra kell számolni a kirajzolandó oldalakat, érdemes a blokkokat nagyobb egységekbe fogni, és minden egységhez egy VBO-t rendelni. Ezek lesznek a chunk-ok. A Minecraft-ban egy chunk mérete 16\*256\*16 blokk, az órákon is megszokott módon az y-tengely a függőleges tengely. (A chunk-méretet érdemes megtartani, ekkora mérettel még jól lehet valós időben számolni a változásokat.) Az alapfeladatban jelenjen meg 10\*10 chunk. (A kirajzolás további optimalizációjáról szól az első választható felada.)  
A kamerát alakítsuk át FPS-kamerává és úgy, hogy ne kelljen nyomva tartani a bal egérgombot a kamera forgatásához. Hogy a kurzor ne tudja elhagyni az ablakot, használjuk az SDL erre kitalált függvényét: [*SDL\_SetRelativeMouseMode*](https://wiki.libsdl.org/SDL_SetRelativeMouseMode), ezzel eltüntethetjük a kurzort. Ezt lehessen ki-be kapcsolni (pl. valamelyik billentyű megnyomásával), hogy a Gui-t is lehessen használni egérrel (ilyenkor a kamera ne mozogjon).

### 1.2 Textúrák (15 pont)


A textúrázást egy textúraatlasszal kell megoldani ([link](http://cg.elte.hu/~robi/nagybead_mc/atlas.png), vagy használható más, akár saját textúra is). A Minecraftban ma már nem így tárolják, hanem dinamikusan generálódik, de most így oldjuk meg.  Az atlasz 16\*16 db azonos méretű résztextúrából áll. A blokkokhoz szerepel az összes szükséges textúra, így csak ezt az egy fájlt kell betölteni, és a megfelelő részéről olvasni. A textúrát *GL\_NEAREST* nagyító szűréssel mintavételezzük (így kapjuk meg a Minecraftra jellemző pixeles textúrákat). Ha a textúra mérete kettőhatvány, akkor a MipMap is jól fog működni (különben összemosódnának az egymás mellett lévő textúrák a kisebb szinteken), ez most teljesül, így használható kicsinyítő szűrésre, pl. *GL\_NEAREST\_MIPMAP\_LINEAR* beállítással. Legyen legalább 5 különböző textúrájú blokk, és ezek között legyen olyan, aminek nem minden oldalán azonos a textúra (pl. fűblokk: az alja föld textúra (1. sor 3.), az oldalai föld \+ fű a tetején (1. sor 4.) és a teteje fű (1. sor 1.).

### 1.3 Tájgenerálás (15 pont)

Egyszerű procedurális világgenerálás megvalósítása. Szükséges valamilyen kétdimenziós magasságtérkép. Ez vehető tetszőleges véletlen algoritmusból: pl. Perlin-zaj vagy Diamond-square algoritmus, de más is megfelel. A világ egy adott (x,z) koordinátájú pontján a magasságtérkép határozza meg, hogy a generálódó legmagasabb blokk milyen magasan (y) legyen. A különböző magasságokban legyenek különböző blokkok, pl. alul kő, feljebb föld és a legtetején egy fűblokk. (A váltások legyenek a maximális magassághoz relatívan meghatározva.)

**Az eddig felsorolt feladatok kötelezőek, ezek nélkül nem értékelhető a beadandó\!**

## 2 Választható feladatok (115 pont)

### 2.1 Rajzolás optimalizálása (15 pont)

Spóroljunk a video-memóriával\! A VAO-ba próbáljunk meg minél kevesebb adatot felvenni, és ezt minél kompaktabb módon átadni. A kirajzolási pozíció vertex-attribútum egész típus is lehet, hiszen a blokkok csak egész koordinátákon lehetnek. Ugyanígy a textúrapozíció is kiszámolható a shaderben egy textúraindexből. Ezt kihasználva kisebb helyen is átadhatók a rajzoláshoz szükséges információk. Ha 4\*32 biten sikerül eltárolni akkor **10 pont** (pl. egyetlen ivec4/uvec4 attribútum), ha 32 biten sikerül eltárolni, akkor **15 pont** (pl. egyetlen int/uint attribútum).

### 2.2 Megjelenítés (10 pont)

#### 2.2.1 Célkereszt és block highlight (5 pont)

A képernyő közepén legyen egy célkereszt. A skyboxnál is használt trükkel a közeli vágósíkra hozható a geometria, így mindig látszani fog. Megvalósítás tetszőleges. Néhány példa/ötlet: két egyszerű vonal-primitív; egy pont-primitív nagy pontmérettel és a kereszten kívüli részek procedurálisan eldobva (discard); egy pont-primitív nagy pontmérettel, textúrával.  

A nézett blokk legyen kiemelve egy “keret”-tel. Ehhez természetesen szükség van a blokkkiütéshez is használt sugárkövetésre (lásd egy kicsit később).

#### 2.2.1 Skybox (5 pont)

A háttérben legyen procedurális skybox: vízszintes alatt sötétkék, vízszintes fölött világoskék, a kettő között egy rövid színátmenet. Hold és nap: a skyboxhoz hasonlóan rajzolva, de rendes texúraként betöltve – a skyboxnál látott trükkel a távoli vágósíkon megjelenítve. A hold és a nap egymással mindig pontosan átellenben keringjenek a világ körül (mint a Minecraftban), az x- vagy a z-tengely körül (**5 pont**).

### 2.3 Interakciók (30 pont)

#### 2.3.1 Blokktörés (20 pont)

Lehessen a blokkokat kiütni a bal egérgombbal. Ehhez a kamerából kell indítani egy sugarat, és meghatározni, hogy melyik az első blokk, amit eltalál. Ezt a blokkot kell törölni, azaz levegővel helyettesíteni. A sugárkövetéshez segítség a feladat végén található.

#### 2.3.2 Blokk lehelyezése és másolása (5 pont)

Blokk kiütése \[bal egérgomb\] mellett lehessen új blokkokat lehelyezni az éppen nézett blokk mellé (a nézett lapra) a jobb egérgomb megnyomásával.  
A középső egérgomb (görgő) megnyomására a lehelyezendő blokk típusa legyen olyan, mint a nézett blokk. A görgővel (is) lehessen a támogatott blokkok között lépkedni.

#### 2.3.3 ImGUI (5 pont)

A gui-n jelenjen meg, hogy milyen típusú blokk van éppen kiválasztva lehelyezésre. A lehelyezendő blokktípus legyen kiválasztható, pl. lenyíló lista. A nap/hold keringési ideje legyen változtatható. Lehessen betölteni új textúraatlaszt képfájlból (legyen legalább két textúra a program mellett, hogy ki lehessen próbálni).

### 2.4 Mentés és betöltés (10 pont)

Lehessen fájlba menteni a világban lévő blokkokat, valamint az így elmentett fájlokat megnyitni a programmal, és betölteni az elmentett világot.

### 2.5 Dinamikus chunk-betöltés/generálás (20 pont)

Ahogy a kamera mozog a messze lévő chunkok ne rajzolódjanak és ne legyenek memóriában sem, valamint még nem látott helyre érve generálódjon “végtelen módon” a táj (**10 pont**). Perzisztencia: ha olyan területre érünk, ami már volt betöltve, az ne generálódjon újra, azaz pl. ha volt rajta változtatás (blokk lehelyezés, blokktörés), akkor azok jelenjenek meg. (**10 pont**).

### 2.6 Játék, “fizika” (20 pont)

Legyen egy karakterünk, amivel mozoghatunk, mint egy fps-játékban (\~Minecraft creative mode repülés nélkül), azaz legyen ütközésvizsgálat, ne tudjunk átmenni a blokkokon. Hasson a karakterre gravitáció is. A játékos hitbox-a egy egyszerű téglatest: 1.8 blokk magas, 0.6 blokk széles és hosszú, oldalai mindig a blokkokkal párhuzamosak, ezzel végezzük az ütközésvizsgálatot. A játékos gyalogsebessége 4.317 blokk/másodperc, futási sebessége 5.612 blokk/másodperc. Ugrás: kb. 1.25 blokk magasra tud a karakter ugrani, tehát egy blokkra fel tud ugrani, de kettőre már nem. Lehessen váltani e között és a szabad repülés között (\~Minecraft spectator mode).

### 2.7 Víz és láva (10 pont)

Legyen víz- és láva-blokk is a játékban. Legyenek animálva (a mellékelt textúraatlasz jobb alsó sarkában van 5-5 frame). Legyen a víz megfelelően átlátszó, azaz látsszanak alatta/mögötte a szilárd blokkok. Legalább arra az esetre működjön helyesen, ha a generált világban egy adott szint alatt minden levegőblokk le van cserélve vízre generáláskor. A részben átlátszó textúrákat mindenképpen utolsóként kell kirajzolni, ezért ezt le kell választani a többi blokk rajzolásától (**10 pont**).

## Segítség az implementációhoz

### Sugárkövetés kockarácson

Egy speciális sugárkövetést kell implementálni, ami a kockákból álló térben keresi az első nem-levegő blokkot, amit metsz a kamerából induló sugár. (Érdemes lehet egy távolsági korlátot is beépíteni “reach”.) Segítség az implementációhoz: a sugáron való haladáskor a különböző oldallapokkal való metszéspontokon kell végigmenni. Ezek gyorsan számolhatók, hiszen az oldallapok párhuzamosak a tengelysíkokkal. Elég tehát mindig a három különböző síkkal megtalálni a metszéspontot, és kiválasztani a legközelebbit, így biztosan nem hagyunk ki egy olyan blokkot sem, amin átmegy a sugár. A képeken lekövethető az algoritmus működése 2D-ben. 3D-ben annyi a változás, hogy a 2 egyenes helyett 3 síkkal kell metszeni.  
