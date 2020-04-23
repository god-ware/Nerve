//dependencies:
//https://github.com/microsoft/cpprestsdk - http, json
//https://www.openssl.org/source/         - sha256

#include "HttpHandler.h"
#include <chrono>
#include <thread>

void displayMenu();
void displayBanner();

int main()
{
    displayBanner();
    json::keep_object_element_order(true);
    std::string line;
    std::string ip;
    std::string regIP;
    std::string ts;
    std::string auth;
    std::string thought;

    ip = HttpHandler::getLocalIPAddress();

    if(!ip.empty())
    {
        BrainChain bc = BrainChain();

        auto control   = std::make_unique<HttpHandler>(U("http://" + HttpHandler::getLocalIPAddress() + ":8081/BrainChain/Start/")   , bc);
        auto httpReg   = std::make_unique<HttpHandler>(U("http://" + HttpHandler::getLocalIPAddress() + ":8081/BrainChain/Register/"), bc);
        auto httpPend  = std::make_unique<HttpHandler>(U("http://" + HttpHandler::getLocalIPAddress() + ":8081/BrainChain/Pend/")    , bc);
        auto httpThink = std::make_unique<HttpHandler>(U("http://" + HttpHandler::getLocalIPAddress() + ":8081/BrainChain/Think/")   , bc);
        auto httpShow  = std::make_unique<HttpHandler>(U("http://" + HttpHandler::getLocalIPAddress() + ":8081/BrainChain/Show/")    , bc);
        auto httpAdd   = std::make_unique<HttpHandler>(U("http://" + HttpHandler::getLocalIPAddress() + ":8081/BrainChain/Add/")     , bc);

        int option = 0;

        do
        {
            displayMenu();
            std::cout << "Select Option: ";
            std::cin >> option;

            //Double-Check is user wants to kill the node
            if(option == 7)
            {
                std::string killswitch;

                std::cout << "Selection this option will kill your node." << std::endl;
                std::cout << "Do you still want to continue?(Y/N): ";
                std::cin  >> killswitch;
                if(killswitch != "Y" && killswitch != "y")
                {
                    std::cout << "Select Option: ";
                    std::cin >> option;
                }
            }

            switch(option)
            {
                case 1:
                    control->post_start();
                    break;
                case 2:
                    std::cout << "What's the IP of the node you want to register with?: ";
                    std::cin >> regIP;
                    std::cout << std::endl;
                    control->post_register(regIP);
                    break;
                case 3:
                    std::cout << "Timestamp: ";
                    std::cin >> ts;
                    std::cout << "Author: ";
                    std::cin >> auth;
                    std::cout << "Thought: ";
                    std::cin >> thought;
                    std::cout << std::endl;
                    control->post_entry(ts, auth, thought);
                    break;
                case 4:
                    control->get_think();
                    break;
                case 5:
                    std::cout << std::endl << bc.getChain() << std::endl;
                    break;
                case 6:
                    std::cout << std::endl << bc.getPeers() << std::endl;
                    break;
                case 7:
                    std::cout << std::endl << "KILLING NODE" << std::endl;
                    break;
                default:
                    std::cout << std::endl << "WRONG OPTION" << std::endl;
            }
        }while(option != 7);
    }
    else
    {
        std::cout << "INTERFACE WLP2S0 NOT FOUND, THEREFORE NO IP FOUND" << std::endl;
        std::cout << "EXITING" << std::endl;
    }

    return 0;
}

void displayMenu()
{
    std::cout << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "                MENU                 " << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "| 1. Start New Chain & Genesis Node |" << std::endl;
    std::cout << "| 2. Join Existing Chain            |" << std::endl;
    std::cout << "| 3. Manually Create Thought        |" << std::endl;
    std::cout << "| 4. Add Brain to Chain             |" << std::endl;
    std::cout << "| 5. Display Chain                  |" << std::endl;
    std::cout << "| 6. Display Registered Peers       |" << std::endl;
    std::cout << "| 7. Kill Node                      |" << std::endl;
    std::cout << "=====================================" << std::endl;
}

void displayBanner()
{
    std::string arr[81];
    arr[1] = "";
    arr[2] = "                                                                    ``........`````                                                                   ";
    arr[3] = "                                                     ```    ````  ``.`.............`                                                                  ";
    arr[4] = "                                           ````....````  `..---. ...` .----.```.---.```........`                                                      ";
    arr[5] = "                                    ``  ``````````  ````````````````  ````    ```````````````       ``                                                ";
    arr[6] = "                               `...`   `....-----...-----..--------.  `.`  ``.----------...   `....`.......                                           ";
    arr[7] = "                              `````           ```````````  `````          ```````````         ``````          `                                       ";
    arr[8] = "                          `...-    ``.......    `.-------  `----.....``  `.-----..  ````......------..`````....                                       ";
    arr[9] = "                       `````````   `````````    ````````   `````````````````````   ````````````````````````````  ```                                  ";
    arr[10] = "                 ```   ``....`     ````...`    ........`  `..................`    ........`               `...` ......``                              ";
    arr[11] = "               ``  ``.......`  `...........``   `.......  `...... ``..........     `........``    `............`.........`                            ";
    arr[12] = "                  `.......`   `...............`   `.....    ``   `...........`      `.......`   `.............`   `........`                          ";
    arr[13] = "              `...````      `...................`   ````    ```......```      `````........``  .................`   `````                             ";
    arr[14] = "             `....```````   `....................`       ```........`     ````............`   `..................`     ``                             ";
    arr[15] = "           `.....`````...`  ``       ``...........`    `..........`     `.....```      ``   `.....................`  `..........``                    ";
    arr[16] = "         .....``      `...     .....   `````........   `.........`    `......`  `....`    .............``..........  ..............`                  ";
    arr[17] = "        `.`       ...`       `.......................   .........     `..............`   `..........``  `.........`  `........``.....                 ";
    arr[18] = "       `.`    ....---....`  .---------------------..`  `---------    -----------..---.  `-----------`  -----------.  .---`.---` `.----.`              ";
    arr[19] = "           `````` ````````  ```````````````````````   ```````````   ````````````` ````` ```````````` ``````````````  ````  ````  ```````              ";
    arr[20] = "     .    .-----.  `..---. `.---------------..``      `.-------.   `------------. .----..----------  `-----------`  -----`  .```.-..````              ";
    arr[21] = "     `   ````````     ``````````````````````            ````````  ```````````````  ````````````````   ```````````   ```````   ````                    ";
    arr[22] = "   ``` `.---.`    ....--------....````````   `....--..` `.-----.  .-------------.. `..---------..`  `.-----------`  ------`  .---.`  `.---.`          ";
    arr[23] = "   ``  `````   ```````````````           ``````````````  ```````  ````````````````    `````````    ``````````````   ```````  ````    ````````         ";
    arr[24] = "    ` `.....``...............`  ``.............. `...... ........  `............`               `...............    .......`      `..........         ";
    arr[25] = "      ............. ```..``   `.................  `...............`  .......``    ````````````    `............`    ..........``.............         ";
    arr[26] = "       `...........``        `..................  `...............`  ....      ```............``   ...........`      `.............` ......`          ";
    arr[27] = "     `.--- `````.-````    ``.------`.---------.  `.--------------` `-```   ```-------------------. ----------`   ``  `.-----------.`  .--`   ```      ";
    arr[28] = "     ``````             ```````````  `````````   ```````````````          ``````````````````````````````````    ````    ``````````    ```    ```      ";
    arr[29] = "     -----------.``    `.-----------`   `.--.`  `..``````.-.```       `.----------------------------------.`   `-----.`  ```````` ``  .----`  .-.     ";
    arr[30] = "     ````````````````   ``````````````                               `````````````````````````````````````   ````````````        ```  ``````  ```     ";
    arr[31] = "      .-------------.`    ...---------.````         ````````     ``..--------------------------------...    `.-----------.... ``.--.  .----` `.-.     ";
    arr[32] = "      ```````` ``````````       ``````````      ````````````    `````  ````````````````````````````        `````````````````  ``````  ````   `````    ";
    arr[33] = "       `.--...  `-------.........-----..     ``.----------.`  `.----.```   `......      ```.-------```..`  ---------------.``..---.` `.---`    ``     ";
    arr[34] = "                   `.................`      ``.......``...`  ``........```         ``````...............   .......................`  .....``  ``      ";
    arr[35] = "          `...```   ````...........`      `.........` `...`  ...............`  ``....................``     ````.................`` `......` `...`    ";
    arr[36] = "           `.....`          `...`      ``........`   `....`  ...............`   `................`      ````                  `..  `.......` .....    ";
    arr[37] = "                   `````            ``..........   `......`  ....`............       ``........```````..........````````   `.....`................    ";
    arr[38] = "                    ```..````       `..........`  `.......`   ``` `...........   ````   `...............................` `...................``..    ";
    arr[39] = "                                   ...........`  `..........    `............. `..........................`````.......... ``.........``` .....`       ";
    arr[40] = "                                   ...........`  ...........   `.........``... `.......................``    `...........`  `.....```  `.......`      ";
    arr[41] = "                                   .....````     `.....``   ``  `.......`  `..``...``............`````````  `............          ````........``     ";
    arr[42] = "                                   `.-------.`   .----.``...--.```.-----. ``.-----` `------.......---------.-----------.` `........----.......`       ";
    arr[43] = "                                    `````````    ````````````````````````    `````   ``````       ``````````````````````  `````````````               ";
    arr[44] = "                                      `.-----.`  .------------......````        `..`        `...`   ``...............`   `.-------.`   `.....         ";
    arr[45] = "                                        ```````  `````````````         ``````    ``````````````````                      ```````````  `````           ";
    arr[46] = "                                          ``.---   `.------. ....    -------..   `.............````                        `.....-.......`            ";
    arr[47] = "                                       `````.....    ```..........   ..........`                              `````   ``         ````                 ";
    arr[48] = "                                        ``........`     ``........``.........``             ``  `.`    ````           ``  ``                          ";
    arr[49] = "                                           ``....`` ............. ``..````           `.``  `   ```              ```` ````    `                        ";
    arr[50] = "                                                   `.............                   `.               ``    ````          ```                          ";
    arr[51] = "                                                                          ``  `..`                  `..```  `..`   ` ```  ``                          ";
    arr[52] = "                                                                          ``     `                `     ```     ```     `                             ";
    arr[53] = "                                                                             `                ``   ````    `                                          ";
    arr[54] = "                                                                                          ```   ``     ````                                           ";
    arr[55] = "                                                                                      `    ```                                                        ";
    arr[56] = "                                                                                      ```                                                             ";
    arr[57] = "                                                                                       `........`                                                     ";
    arr[58] = "                                                                                          ```````                                                     ";
    arr[59] = "                                                                                           `......`                                                   ";
    arr[60] = "                                                                                             `.....                                                   ";
    arr[61] = "                                                                                              `.....``                                                ";
    arr[62] = "                                                                                                `....`                                                ";
    arr[63] = "                                                                                                  ``                                                  ";
    arr[64] = "";
    arr[65] = "             ./syyyys+-.:s      ./oyyyyys+-`   `:+ooooooooo/:.`   -+ooooo/-`/oooooo+:   `/+o+-  .h.       :/ooooooooo+/-.   .:+oooooooooooooo.        ";
    arr[66] = "           :yNMy:.`.-+hmNd    :hNMh:.`.-oNNdo`  `-MMMMM+::ohMNdo. `oMMMMN.` .+NMMMM+`    .yN/`  yMy       `.NMMMMo::odMMmh: ``-MMMMM/-::/sdMM/        ";
    arr[67] = "         `sNMMy       `oMd  `yMMMh`      :MMMm:   NMMMM-    oMMMm/ `mMMMM/    oMMMMy     `ms   :MMM-        dMMMM/   `dMMMN/   NMMMM-     `+M/        ";
    arr[68] = "        `yMMMN`         sd  hMMMM-        hMMMN:  NMMMM-     hMMMM+ :MMMMm`   `NMMMM:    +N`  `mMMMd        dMMMM/    oMMMMd   NMMMM-   /`  y/        ";
    arr[69] = "        +MMMMh          `+ +MMMMN         oMMMMd  NMMMM-     /MMMMN. hMMMMo    oMMMMd   `N+   oMMMMM+       dMMMM/    sMMMMd   NMMMM-   m`  ``        ";
    arr[70] = "        NMMMMs             dMMMMm         /MMMMM- NMMMM-     .MMMMMo .NMMMN.  `dNMMMM/  sm`  .NoMMMMN`      dMMMM/   .mMMMN:   NMMMM- .sM`            ";
    arr[71] = "       -MMMMM+             NMMMMd         :MMMMM/ NMMMM-      MMMMMy  oMMMMy  om/MMMMm`.N/   hh sMMMMs      dMMMMs:/smMNmy-    NMMMMyhmMM`            ";
    arr[72] = "       -MMMMM+    .--------NMMMMd         /MMMMM/ NMMMM-      MMMMMy  `mMMMM:-N: hMMMMoyh   /N. `NMMMM-     dMMMMssMMMMm.      NMMMM/:+mM`            ";
    arr[73] = "       `MMMMMo    -:dMMMMM/hMMMMm         +MMMMM. NMMMM-     .MMMMM+   /MMMMdds  .MMMMNM-  `mo   +MMMMh     dMMMM/ yMMMMy`     NMMMM-  .N`            ";
    arr[74] = "        hMMMMy      +MMMMd :MMMMM`        sMMMMh  NMMMM-     /MMMMm`    hMMMMm`   sMMMMy   sMsooooNMMMM/    dMMMM/  yMMMMy     NMMMM-   y`   `s       ";
    arr[75] = "        .mMMMN`     +MMMMh  oMMMM/       `mMMMd.  NMMMM-     dMMMN-     -MMMM:    `NMMN.  -N/-----/MMMMm`   dMMMM/  `yMMMMs    NMMMM-       `yh       ";
    arr[76] = "         .hMMMy`    +MMMMh   /mMMN-     `sMMNy`   NMMMM:   `sMMNy.       sMMs      +MMo  `dh       hMMMMs   dMMMM/   `hMMMMs   MMMMM:     `:dM+       ";
    arr[77] = "           :yNMdo/:/hMNNh/    `+hNNs/:/odNms-  `-oNMMMMdssymmh+.         `Nm`       mm` -yMs-     -sMMMMNo-/NMMMMh:.  `hMMMMy-oMMMMMdsosshmMMN.       ";
    arr[78] = "              .:++++/:.          `-/+++/:.     `----------.`              :-        --  -----    `-----------------.   `---------------------.        ";
    arr[79] = "                                                                                                                                                 ";
    arr[80] = "";

//    for(int i = 0; i <= 80; i++)
//    {
//        std::cout << arr[i] << std::endl;
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    }
//    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//    for (int n = 0; n < 10; n++)
//        printf( "\n\n\n\n\n\n\n\n\n\n" );
}

