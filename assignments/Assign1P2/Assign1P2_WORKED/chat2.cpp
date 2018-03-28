#include <Arduino.h>
// constant variables
const int      ANALOG_PIN = 0;   
const int      DigitalPin = 13;                                                     // use pin 0 on arduino
const int      BUFF       = 500;                                                    // max characters stored in one text
const uint32_t prime      = 2147483647UL;                                                   // diffie-Hellman prime
const uint32_t generator  = 16807UL;                                                  // diffie-hellman generator


// function prototypes
uint32_t genPrivateKey(void );
int getInput(char* inputstr );
uint32_t longmulmod( uint32_t , uint32_t , uint32_t );
uint32_t longpowmod( uint32_t , uint32_t , uint32_t );
bool isServer(void );
bool waitOnSerial3( uint8_t, long );
void uint32ToSerial3(uint32_t );
void showUint32ToSerial3(uint32_t );
uint32_t uint32FromSerial3(void );
uint32_t client(uint32_t , uint32_t );
uint32_t server(uint32_t , uint32_t );
uint32_t getNextKey(uint32_t );
void send(char*, uint32_t);
void receive(uint32_t);
char enDecrypt(char, char );
///test function prototypes
void test_all(void);
void test_longpowmod_case( uint32_t, uint32_t, uint32_t, uint32_t );
void test_longpowmod(void);
void test_longmulmod_case( uint32_t, uint32_t, uint32_t, uint32_t );
void test_longmulmod(void);
void test_genPrivateKey(void);
void test_getInput(char* inputstr);
void test_isServer(bool);


int main()
{
	init();
    pinMode(DigitalPin, INPUT);
	Serial.begin(9600);
	Serial3.begin(9600);
    // // test
    // uint32_t ans = longpowmod(16807, 1259760335, 2147483647);
    // Serial.println(ans);
    // while (true) {}
    // // test end
    
    uint32_t cPublicKey = 0;
    uint32_t sPublicKey = 0;
    uint32_t secretKeySource = 0;
    char inputstr[BUFF + 1];

    const uint32_t privateKey = genPrivateKey();
    
    if ( isServer() )
    {
        // act as a server
        Serial.println(">> I am a server.");
        sPublicKey = longpowmod(generator, privateKey, prime);
        cPublicKey = server(cPublicKey, sPublicKey);
        secretKeySource = longpowmod(cPublicKey, privateKey, prime);
        Serial.println(">> Server is ready to communicate.");
    }
    else
    {
        // act as a client
        Serial.println(">> I am a client.");      
        cPublicKey = longpowmod(generator, privateKey, prime);
        sPublicKey = client(cPublicKey, sPublicKey);
        secretKeySource = longpowmod(sPublicKey, privateKey, prime);
        Serial.println(">> Client is ready to communicate.");
    }


    //////Start communication//////
    
    while (true)
    {
        // Right now when msg comes in, it will somehow wait in a queue,
        // when program goes to next iteration, the incoming msg in the
        // queue will enable Serial3.available() and hence user can
        // receive msg.
        // Limits: if user already have some words typed in the console and
        // meanwhile there is a incoming msg, the msg need to wait in the
        // queue until the typed in words send out.


        if ( Serial3.available() )
        {
            receive(secretKeySource);
        }
        else if ( Serial.available() )
        {
            send(inputstr, secretKeySource);
        }
    }




	Serial.end();
	Serial3.end();


	return 0;
}


/** 
 * accept a string and return the number of characters get stored in
 * the string after user enter <return>
 * use a string to temporarily store the msg that user types in,
 * thus allow user to "modify" (flush out) words that is mistakenly
 * typed in.
 * Limits: User can only REPLACE instead of DELETE completely
 *
 * @param    char* inputstr        string that is used to store the input msg
 * @return   int                   number of characters that user enters or 
 *                                  size of BUFF if exceeded the BUFF size 
 */
int getInput(char* inputstr)
{
    char in;
    int index = 0;
    memset(inputstr, 0, sizeof(inputstr));                      // clean up existing values in inputstr array

    while (true)
    {
        // wait for input
        while ( !Serial.available() ) {}                        //do nothing when not available
        if ( Serial.available() > 0)
        {
            if ( BUFF == index)
            {
                inputstr[index] = '\0';
                Serial.println();
                return index;
            }
            in = Serial.read();
            // when user hit <enter> OR user exceeds 100 character limits,
            // only get the first 100 characters
            if ('\r' == in || '\n' == in)      // end of input
            {
                inputstr[index] = '\0';
                Serial.println();
                return index;
            }
            Serial.print(in);
            inputstr[index++] = in;
        }
    }
}

/**
 * generate 32 bit private key to improve security
 * the random number is generated by the fluctuation of analog pin 
 * when it does not connect to anything
 * @return uint32_t the private key generated
 */
uint32_t genPrivateKey()
{

    const int BITS_REQUIRED = 32;                                               // 16-bit number is required in this assignment
    Serial.println(">> Generating private key...");
    uint32_t res = 0;                                                           // store the final result
    int temp = 0;                                                               // store the analogRead value in each loop
    for (int i = 0; i < BITS_REQUIRED; ++i)                                     // loop to get random number 
    {
        temp = analogRead(ANALOG_PIN);
        // first left shift res one place in order to create "room"
        // for next digit. Then use 1 as mask to get the least significant
        // digit in temp and add it to res.
        res = (temp & 1) + ( res << 1 );                                            // ( temp & 1) is least sig digit
        delay(100);                                                             // longer delay for "more random" result
    }
    Serial.println(">> Private key generated.");

    return res;
}


// TODO: generalize, test for critical values
uint32_t longmulmod( uint32_t a, uint32_t b, uint32_t m )
{
    uint32_t result  = 0;
    uint32_t product = b % m;
    int      ithVal     = 0;
    // while ( a )
    // {
    //     result = (result + ( product * ( a & 1 ) )  ) % m;
    //     product = ( product * 2 ) % m;
    //     a >>= 1;
    // }
    
    while ( a )
    {
        ithVal = a & 1;
        if ( ithVal )
        {
            result = ( result + ithVal * product ) % m;
        }
        product = (product * 2 ) % m;   // for next iteration
        a = a >> 1;
    }

    return result;
}

// uint32_t longpowmod( uint32_t a, uint32_t b, uint32_t m )
// {

//     // test
//     m = 2147483647;
//     // test end

//     // test
//     Serial.println("in function arguments:");
//     Serial.println(a);
//     Serial.println(b);
//     Serial.println(m);
//         // // test end   
//     // test end
//     uint32_t result = 1;
//     a = a % m;
//     while (b)
//     {
//         // test if the right most binary representation of b is 1
//         if ( b & 1 ) 
//         {
//             result = longmulmod( result, a, m);          // result * a % m;
//             // test
//             Serial.print("result is ");
//             Serial.println(result);
//             // test end
//         }
//         // right shift b by 1 in order to get ready for the coputation 
//         // in next iteration
//         b = b >> 1;
//         a =  longmulmod(a, a, m);                       // a * a % m
//     }

//     return result;
    
// }
uint32_t longpowmod( uint32_t a, uint32_t b, uint32_t m )
{
    m = 2147483647ul;

    uint32_t result = 1;
    a = a % m;
    while (b)
    {
        // test if the right most binary representation of b is 1
        if ( b & 1 )
        {
            result = longmulmod( result, a, m);          // result * a % m;
//            // test
//            cout << "result is ";
//            cout << result << endl;
//            // test end
        }
        // right shift b by 1 in order to get ready for the coputation
        // in next iteration
        b = b >> 1;
        a =  longmulmod(a, a, m);                       // a * a % m
    }

    return result;

    // if (b == 0)
    //     return 1;
    // uint32_t result = longpowmod(a, b >> 1, m);
    // result = longmulmod(result, result, m);
    // return (b & 1)? longmulmod(a, result, m): result;
}


/** Waits for a certain number of bytes on Serial3 or timeout 
 * @param nbytes: the number of bytes we want
 * @param timeout: timeout period (ms); specifying a negative number
 *                turns off timeouts (the function waits indefinitely
 *                if timeouts are turned off).
 * @return True if the required number of bytes have arrived.
 */
bool waitOnSerial3( uint8_t nbytes, long timeout ) {
    unsigned long deadline = millis() + timeout;        //wraparound not a problem
    while (Serial3.available() < nbytes && (timeout < 0 || millis() < deadline)) 
    {
        delay(1); // be nice, no busy loop
    }
    return Serial3.available() >= nbytes;
    }


bool isServer()
{
    return HIGH == digitalRead( DigitalPin );
}


/** Writes an uint32_t to Serial3, starting from the least-significant
 * and finishing with the most significant byte. 
 * @param uint32_t num an uint32_t integer to be sent
 */
void uint32ToSerial3(uint32_t num) {
    Serial3.write((char) (num >> 0));
    Serial3.write((char) (num >> 8));
    Serial3.write((char) (num >> 16));
    Serial3.write((char) (num >> 24));
}


/** Reads an uint32_t from Serial3, starting from the least-significant
 * and finishing with the most significant byte. 
 */
uint32_t uint32FromSerial3() {
    uint32_t num = 0;
    num = num | ((uint32_t) Serial3.read()) << 0;
    num = num | ((uint32_t) Serial3.read()) << 8;
    num = num | ((uint32_t) Serial3.read()) << 16;
    num = num | ((uint32_t) Serial3.read()) << 24;
    return num;
}


uint32_t client(uint32_t cPublicKey, uint32_t sPublicKey)
{
    const int Timeout = 1000;
    const char CR = 'C';
    const char ACK = 'A';
    enum CState { Start, WaitingForAsk, DataExchange };
    CState currentState = Start;

    while (true)
    {
        if ( Start == currentState)
        {
            // recv:-
            // action: send CR(ckey)
            // TODO: test this
            Serial3.write( CR );
            uint32ToSerial3(cPublicKey);            
            currentState = WaitingForAsk;
            // test
            Serial.print("msg sent: ");
            Serial.print(CR);
            Serial.println(cPublicKey);
            // test end
        }
        else if ( WaitingForAsk == currentState )
        {
            bool ok = waitOnSerial3(5, Timeout);
          
            if ( ok && ACK == Serial3.read() ) 
            {
                // recv: ACK(skey)
                // action: store key, send ACK
                sPublicKey = uint32FromSerial3();
                //test
                Serial.print("sPublicKey received: ");
                Serial.println(sPublicKey);
                //test end
                Serial3.write( ACK );
                currentState = DataExchange;
            }
            else
            {
                // recv: timeout(1000)
                // action :-
                currentState = Start;
                
            }
        }
        else if ( DataExchange ==  currentState )
        {
             return sPublicKey;
        }
    }
   
}


uint32_t server(uint32_t cPublicKey, uint32_t sPublicKey)
{
    const int Timeout = 1000;
    const char CR = 'C';
    const char ACK = 'A';
    enum SState { Listen, WaitingForKey1, WaitingForKey2, WaitForAsk, DataExchange };
    SState currentState = Listen;

    Serial.println(">> Listening to client...");
    while (true)
    {
        if (Listen == currentState )
        {
            // recv: CR
            // action:-
            bool ok = waitOnSerial3(5, Timeout);
            if ( ok && CR == Serial3.read() )
                currentState = WaitingForKey1;
        }
        else if ( WaitingForKey1 == currentState )
        {
            // recv: ckey
            // action: store ckey
            //          send  ACK(skey)
            cPublicKey = uint32FromSerial3();
            // test
            Serial.print("cPublicKey reveived: ");
            Serial.println(cPublicKey);
            // test end
            
            // send ACK(skey)
            Serial3.write( ACK );
            uint32ToSerial3(sPublicKey);
            currentState = WaitForAsk;
        }
        else if ( WaitForAsk == currentState )
        {
            char ch = Serial3.read();
            if ( ACK == ch )
            {
                // recv: ACk
                // action:-
                currentState = DataExchange;
            }
            else if ( CR == ch )
            {
                // recv: CR
                // action:-
                currentState = WaitingForKey2;
            }
        }
        else if (WaitingForKey2 == currentState)
        {
            // recv: ckey
            // action: store ckey
            cPublicKey = uint32FromSerial3();
            currentState = WaitForAsk;
            // test
            Serial.print("cPublicKey reveived: ");
            Serial.println(cPublicKey);
            // test end
        }
        else if ( DataExchange == currentState )
        {
            Serial.println(">> data exchange established.");
            return cPublicKey;
        }
        else
        {
            currentState = Listen;
        }
    }

}


/** Implements the Park-Miller algorithm with 32 bit integer arithmetic 
 * @return ((current_key * 48271)) mod (2^31 - 1);
 * This is linear congruential generator, based on the multiplicative
 * group of integers modulo m = 2^31 - 1.
 * The generator has a long period and it is relatively efficient.
 * Most importantly, the generator's modulus is not a power of two
 * (as is for the built-in rng),
 * hence the keys mod 2^{s} cannot be obtained
 * by using a key with s bits.
 * Based on:
 * http://www.firstpr.com.au/dsp/rand31/rand31-park-miller-carta.cc.txt
 */
// TODO: prove that this func has no overflow
uint32_t getNextKey(uint32_t current_key) {
  const uint32_t modulus = 0x7FFFFFFF; // 2^31-1
  const uint32_t consta = 48271; // we use that this is <=15 bits
  uint32_t lo = consta*(current_key & 0xFFFF);  
  uint32_t hi = consta*(current_key >> 16); 
  lo += (hi & 0x7FFF)<<16;
  lo += hi>>15;
  if (lo > modulus) lo -= modulus;
  return lo;
}


void send(char* inputstr, uint32_t secretKeySource)
{

    Serial.print("Joanne: ");                                               // sender's name
    int numOfChar = getInput(inputstr);                                     // 'numOfChar' stores how many characters stored in inputstr.

    uint32_t currentKeySource = secretKeySource; 
    char currentKey;
    uint8_t encryptedArr[ numOfChar + 1 ];                                       // declare array for encrypted characters
    for (int i = 0; i < numOfChar + 1; ++i)
    {
        currentKey = currentKeySource & 0xFF;                                 // current key is a char so it only take 8 bits
        encryptedArr[i] = enDecrypt(inputstr[i], currentKey);          // encrypt all characters with sharedSecretKey

        currentKeySource = getNextKey(currentKeySource);
    }

    int sendBytes = Serial3.write(encryptedArr, numOfChar + 1 );                    // how bytes are sent
    // test
    Serial.print("Bytes sent: ");
    Serial.println(sendBytes);
    // test end

    if (BUFF == numOfChar)                                                         // max character has reached
    {
        // if inputstr does not store all msg from user input, recursively
        // call send() to send exceeding msg
        send(inputstr, secretKeySource);
    }
}


/**
 * called when Serial3.available() is not 0 i.e. there is a msg snet to you from other user
 *
 * @param   char sharedSecretKey        used to decrypt receiving msg
 */
void receive(uint32_t secretKeySource)
{
    char ch;
    Serial.print("Paul: ");                                                     //user B's name
    uint32_t currentKeySource = secretKeySource; 
    char currentKey;
    while (true)
    {
        if ( Serial3.available() )                                              //if available, read characters from user_B,
        {
            currentKey = currentKeySource & 0xFF;
            ch = Serial3.read();
            ch = enDecrypt(ch, currentKey);

            // get next key for next iteration
            currentKeySource = getNextKey(currentKeySource);
            if ( '\0' == ch )                                                   //last character detected
            {
                Serial.println();                                               //print blank and exit
                return;
            }
            Serial.print(ch);                                                   //print characters as usual
            while ( !Serial3.available() ) {}                                   //comes here (do nothing) when not available
        }
    }
}


/**
 * encrypt or decrypt the sending character using theroy of Diffie–Hellman key exchange:
 * encrypt or decrypt one character with a certain shared secret key
 * using XOR ( ^ ) operation.
 *
 * @param   char ch                 the character to be encrypted/decrypted   
 * @param   char sharedSecretKey    the secret key used
 * @return  char                    the encrypted/decrypted character
 */
char enDecrypt(char ch , char sharedSecretKey )      
{                                                                               
    return ch ^ sharedSecretKey;
}


///////////////////////////////////////////////
/////////////////////TEST//////////////////////
///////////////////////////////////////////////
void test_longpowmod_case( uint32_t a, uint32_t b, uint32_t m , uint32_t e)
{
    uint32_t out = longpowmod(a, b, m);
    if (out != e)
    {
        Serial.print("Input: ");
        Serial.print(a); Serial.print(" "); 
        Serial.print(b); Serial.print(" ");
        Serial.println(m);
        Serial.print("Your result: "); Serial.println( out );
        Serial.print("expectation result: "); Serial.println( e );
        Serial.println("FAIL!!");
        Serial.println("=============================================");
    }
    
}


void test_longpowmod()
{
    test_longpowmod_case(5, 10, 37, 30);
    test_longpowmod_case(5, 8, 37, 16);
    test_longpowmod_case(5, 100, 37, 7);
    test_longpowmod_case(16807UL, 124112535UL, 2147483647UL, 2117462694UL);
    test_longpowmod_case(161207UL, 12422512UL, 2123453647UL, 2054842787UL);
    test_longpowmod_case(586542895, 1925239802, 2147483647, 941978530);
    test_longpowmod_case(1076635382, 1087022722, 2147483647, 1793039265);
    test_longpowmod_case(1357211968, 2348089557, 2147483647, 1760421744);
    test_longpowmod_case(16807, 395589541, 2147483647, 2105182053);
    test_longpowmod_case(16807, 3957940345, 2147483647, 1611071818);
    // TODO more larger tests
    Serial.println("test_longpowmod test complete");
}


void test_longmulmod_case( uint32_t a, uint32_t b, uint32_t m , uint32_t exp)
{
    uint32_t out = longmulmod( a, b, m);
    if ( exp != out )
    {
        Serial.print("Input: ");
        Serial.print(a); Serial.print(" "); 
        Serial.print(b); Serial.print(" ");
        Serial.println(m);
        Serial.print("Your result: "); Serial.println( out );
        Serial.print("expectation result: "); Serial.println( exp );
        Serial.println("FAIL!!");
        Serial.println("=============================================");
    }
}


void test_longmulmod()
{
 
    test_longmulmod_case(5, 10, 37, 13);
    test_longmulmod_case(5, 8, 37, 3);
    test_longmulmod_case(5, 100, 37, 19);
    test_longmulmod_case(16807, 124112535, 2147483647, 752754508);
    test_longmulmod_case(161207, 12422512, 2123453647, 179102863);


    Serial.println("test_longmulmod test complete.");
    
}

void test_genPrivateKey()
{
    for (int i = 0; i < 20; ++i)
    {
        Serial.println( genPrivateKey() );
        delay(500);
    }
    Serial.println("test_genPrivateKey test complete");
    
}


void test_getInput(char* inputstr)
{
    Serial.println("test_getInput test: Please enter a sentence:");
    getInput(inputstr);
    Serial.println(inputstr);
    Serial.println("Serial.println test complete");
    
}


void test_isServer(bool ok)
{
    if (isServer() == ok )
    {
        Serial.println("test_isServer test pass!!");
    }
    else
    {
        Serial.println("FAIL on test_isServer ");
    }
    Serial.println("test_isServer test complete");
}
