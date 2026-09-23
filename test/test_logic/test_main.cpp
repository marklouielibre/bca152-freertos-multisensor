#include <unity.h>

#include "alarm.h"
#include "system_state.h"

// ---------- Temperature Alarm (5) ----------

void test_temp_below_lower()
{
    TEST_ASSERT_EQUAL(AlarmState::LOW_TEMPERATURE,
                      evaluateTemperature(19.9f));
}

void test_temp_exactly_lower()
{
    TEST_ASSERT_EQUAL(AlarmState::NORMAL,
                      evaluateTemperature(20.0f));
}

void test_temp_normal()
{
    TEST_ASSERT_EQUAL(AlarmState::NORMAL,
                      evaluateTemperature(25.0f));
}

void test_temp_exactly_upper()
{
    TEST_ASSERT_EQUAL(AlarmState::NORMAL,
                      evaluateTemperature(30.0f));
}

void test_temp_above_upper()
{
    TEST_ASSERT_EQUAL(AlarmState::HIGH_TEMPERATURE,
                      evaluateTemperature(30.1f));
}

// ---------- Display Navigation (4) ----------

void test_next_temperature()
{
    TEST_ASSERT_EQUAL(DisplayMode::HUMIDITY,
                      nextDisplayMode(DisplayMode::TEMPERATURE));
}

void test_next_wraparound()
{
    TEST_ASSERT_EQUAL(DisplayMode::TEMPERATURE,
                      nextDisplayMode(DisplayMode::MOTION));
}

void test_previous_humidity()
{
    TEST_ASSERT_EQUAL(DisplayMode::TEMPERATURE,
                      previousDisplayMode(DisplayMode::HUMIDITY));
}

void test_previous_wraparound()
{
    TEST_ASSERT_EQUAL(DisplayMode::MOTION,
                      previousDisplayMode(DisplayMode::TEMPERATURE));
}

// ---------- System State (4) ----------

void test_active_no_timeout()
{
    TEST_ASSERT_EQUAL(SystemState::ACTIVE,
        evaluateSystemState(SystemState::ACTIVE, false, false));
}

void test_active_timeout()
{
    TEST_ASSERT_EQUAL(SystemState::INACTIVE,
        evaluateSystemState(SystemState::ACTIVE, false, true));
}

void test_inactive_no_motion()
{
    TEST_ASSERT_EQUAL(SystemState::INACTIVE,
        evaluateSystemState(SystemState::INACTIVE, false, false));
}

void test_inactive_motion()
{
    TEST_ASSERT_EQUAL(SystemState::ACTIVE,
        evaluateSystemState(SystemState::INACTIVE, true, false));
}

void setUp()
{
}

void tearDown()
{
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_temp_below_lower);
    RUN_TEST(test_temp_exactly_lower);
    RUN_TEST(test_temp_normal);
    RUN_TEST(test_temp_exactly_upper);
    RUN_TEST(test_temp_above_upper);

    RUN_TEST(test_next_temperature);
    RUN_TEST(test_next_wraparound);
    RUN_TEST(test_previous_humidity);
    RUN_TEST(test_previous_wraparound);

    RUN_TEST(test_active_no_timeout);
    RUN_TEST(test_active_timeout);
    RUN_TEST(test_inactive_no_motion);
    RUN_TEST(test_inactive_motion);

    return UNITY_END();
}