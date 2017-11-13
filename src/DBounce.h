/**
Fast, low resource debouncing class for Arduino.

*/
#ifndef __DBOUNCE_H
#define __DBOUNCE_H

#if __cplusplus < 199711L || defined(__AVR)
  // somehow avr g++ is old fashioned
  #include <stdint.h>
  #include <string.h>
#else
  #include <cstdint>
  #include <cstring>
#endif

#if !defined(__DBNC_REGISTER_SIZE)
  #if defined(__AVR)
  // do some 8-bit optimations (needs less RAM and less flash)
    #define __DBNC_REGISTER_SIZE  8
  #else
  // use the 32-bit implementation instead, even with 16-bit controllers
    #define __DBNC_REGISTER_SIZE  32
  #endif
#endif

/// number of elements of a static array
#define __DBNC_N(array) 	(sizeof array / sizeof *array)


/**************************************************************************************************
The debounce template class.

@param BM_T        bitmask type. It's type must have at least N bits. So use
                   - uint8_t for 8 and less inputs
                   - uint16_t for 16 and less inputs
                   - uint32_t for 32 and less inputs
                   - uint64_t for 64 and less inputs
@param N           number of inputs. Normally you can use the @__DBNC_N() macro.
@param debounceMS  debounce interval as milliseconds, can be 0 when @lockoutMS > 0.
@param lockoutMS   lock-out interval as milliseconds, can be 0 when @debounceMS > 0.

@note The value of @debounceMS and @lockoutMS has to fit into the timing_t type.
Usefull values are far below 255ms normally.
*/
template<typename BM_T, uint8_t N, uint32_t debounceMS, uint32_t lockoutMS>
class DBounce
{
  public:
  typedef BM_T   bitmask_t;
#if __DBNC_REGISTER_SIZE <= 8
  typedef uint8_t     timing_t;
  #define __DBNC_TIMING_MAX  UINT8_MAX

#elif __DBNC_REGISTER_SIZE <= 16
  typedef uint16_t     timing_t;
  #define __DBNC_TIMING_MAX  UINT16_MAX

#else
  typedef uint32_t    timing_t;
#endif

  private:
  const uint8_t *pins;
#if __DBNC_REGISTER_SIZE < 32
  uint32_t m_now;
#endif
  timing_t timestamp[N];
  bitmask_t m_state;
  bitmask_t m_change;
  bitmask_t m_lock;

  public:
  /**********************************************************************************************
  c'tor, the only one

  @param pins[N] 		[IN] static array of pin numbers of dimension N
  */
  DBounce(const uint8_t pins[N])
    : pins(pins), m_state(0), m_change(0), m_lock(0)
  {
  };


  /**********************************************************************************************
  Setup GPIOs and get initial state.

  You have to call this function within Arduinos setup() function.

  @param _pinMode   [IN] like parameter for pinMode(), normally INPUT or INPUT_PULLUP
  */
  void setup(uint8_t _pinMode)
  {
#if __DBNC_REGISTER_SIZE < 32
    m_now = millis();
#else
    uint32_t now = millis() + debounceMS;
#endif

    for(uint8_t i = 0; i < N; i++)
    {
      pinMode(pins[i], _pinMode);
#if __DBNC_REGISTER_SIZE < 32
      timestamp[i] = debounceMS;
#else
      timestamp[i] = now;
#endif
      if (digitalRead(pins[i])) m_state |= 1 << i;
    }
  };


  /**********************************************************************************************
  Update all pin states.

  Call this function exactly once in Arduinos loop() function.

  @return -
  */
  void update()
#if __DBNC_REGISTER_SIZE < 32
  {
    m_change = 0;

    uint32_t now = millis();
    timing_t tdiff = 0;
    if (now != m_now)
    {
      uint32_t diff = now - m_now;
      tdiff = (diff > __DBNC_TIMING_MAX) ? __DBNC_TIMING_MAX : diff;
      m_now = now;
    }

    for(uint8_t i=0; i < N; i++)
    {
      if (tdiff != 0) {
        if (tdiff <= timestamp[i]) {
          timestamp[i] -= tdiff;
        } else {
          timestamp[i] = 0;
        }
      }

      if(m_lock & (1<<i)) {
        if (timestamp[i] != 0) { continue; }
        timestamp[i] = debounceMS;

        m_lock &= ~(1<<i);
      }

      if(digitalRead(pins[i]) != ((m_state & (1<<i)) != 0)) {
        if (timestamp[i] != 0) { continue; }
        timestamp[i] = lockoutMS;

        m_change |= (1 << i);
        continue;
      }
      timestamp[i] = debounceMS;
    }

    // change all states
    m_state ^= m_change;
    m_lock |= m_change;
  }

#else
  // the standard 32-bit implementation is much more straight forward
  {
    m_change = 0;
    uint32_t now = millis();

    for(uint8_t i=0; i < N; i++) {
      if(m_lock & (1<<i)) {
        if (now < timestamp[i]) { continue; }
        timestamp[i] = now + debounceMS;
        m_lock &= ~(1<<i);
      }

      if(digitalRead(pins[i]) != ((m_state & (1<<i)) != 0)) {
        if (timestamp[i] > now) { continue; }
        // state really changed
        timestamp[i] = now + lockoutMS;

        m_change |= (1 << i);
        continue;
      }
      timestamp[i] = now + debounceMS;
    }

    // change all states
    m_state ^= m_change;
    m_lock |= m_change;
  }
#endif
  ;


  /**********************************************************************************************
  rising edge event

  @param idx   [IN] button array index
  @retval true  rising edge detected
  */
  bool rose(uint8_t idx)
  {
    return (m_state & m_change & ((bitmask_t)1u << idx)) != 0;
  };


  /**********************************************************************************************
  falling edge event

  @param idx   [IN] button array index
  @retval true  falling edge detected
  */
  bool fell(uint8_t idx)
  {
    return (~m_state & m_change & ((bitmask_t)1u << idx)) != 0;
  };


  /**********************************************************************************************
  level change occured

  @param idx   [IN] button array index
  @retval true  change deteced
  */
  bool change(uint8_t idx)
  {
    return (m_change & ((bitmask_t)1u << idx)) != 0;
  };


  /**********************************************************************************************
  get pin state

  @param idx     [IN] button array index
  @retval false  low input level
  @retval true   high input level
  */
  bool read(uint8_t idx)
  {
    return (m_state & ((bitmask_t)1u << idx)) != 0;
  };


  inline bitmask_t read() { return m_state; };
  inline bitmask_t change() { return m_change; };
  inline bitmask_t rose() { return m_state & m_change; };
  inline bitmask_t fell() { return (~m_state) & m_change; };
};

#endif
