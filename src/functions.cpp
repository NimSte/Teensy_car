#include <FlexCAN_T4.h>
#include <Arduino.h>
#include "ecu_functions.h"
#include "ecu_defines.h"
// -------------------------------------------------------------
void Status_Print() {
  Serial.println("=====================system status=======================");
  Serial.print("Throttle: ");
  Serial.print(Throttle);
  Serial.println(" [%]");
  Serial.print("Brake: ");
  Serial.print(Brake);
  Serial.println(" [%]");
  Serial.print("current: ");
  Serial.print(Current_meas);
  Serial.println(" [mA]");
  Serial.print("Voltage measure 1: ");
  Serial.print(Voltage_meas1);
  Serial.println(" [mV]");
  Serial.print("Voltage measure 2: ");
  Serial.print(Voltage_meas2);
  Serial.println(" [mV]");
  Serial.print("Voltage measure 3: ");
  Serial.print(Voltage_meas3);
  Serial.println(" [mV]");
  Serial.print("Temperature measure: ");
  Serial.print(Temperature_meas);
  Serial.println(" [C]");
  Serial.print("Power measure: ");
  Serial.print(Power_meas);
  Serial.println(" [W]");
  Serial.print("Battery state: ");
  Serial.println(Battery_state);
  Serial.print("Battery SOC percent: ");
  Serial.print(Battery_SOC_percent);
  Serial.println(" [%]");
  Serial.print("Battery Voltage: ");
  Serial.print(Battery_Voltage);
  Serial.println(" [mA]");
  Serial.print("Charger flag: ");
  Serial.println(Charger_flags);
  
  Serial.print("Motor torqe: ");
  Serial.print(Motor_Torqe);
  Serial.println(" [%]");
  Serial.print("Motor on flag: ");
  Serial.println(Motor_On);
  Serial.print("Motor voltage: ");
  Serial.print(Motor_Voltage);
  Serial.println(" [V]");
  Serial.print("Voltage implausibility: ");
  Serial.println(voltage_implausibility);
  Serial.println("==========================END============================");
}


void Print_CanMsg(const CAN_message_t &msg) {
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}
void PedalControllerMB(const CAN_message_t &inMsg) {
  Brake = inMsg.buf[2];
  if (inMsg.buf[0] == 0x20) {
    TPS_Implausibility = 1;
    Serial.println("Implausibility");
    Throttle = 0;
  }
  else
    Throttle = inMsg.buf[1];
}
void CurrentMeasMB(const CAN_message_t &inMsg) {
  Current_meas = (inMsg.buf[2] << 24) + (inMsg.buf[3] << 16) + (inMsg.buf[4] << 8) + inMsg.buf[5];
}
void VoltageMeasure1MB(const CAN_message_t &inMsg) {
  Voltage_meas1 = (inMsg.buf[2] << 24) + (inMsg.buf[3] << 16) + (inMsg.buf[4] << 8) + inMsg.buf[5];
}
void VoltageMeasure2MB(const CAN_message_t &inMsg) {
  Voltage_meas2 = (inMsg.buf[2] << 24) + (inMsg.buf[3] << 16) + (inMsg.buf[4] << 8) + inMsg.buf[5];
}
void VoltageMeasure3MB(const CAN_message_t &inMsg) {
  Voltage_meas3 = (inMsg.buf[2] << 24) + (inMsg.buf[3] << 16) + (inMsg.buf[4] << 8) + inMsg.buf[5];
}
void TemperatureMeasureMB(const CAN_message_t &inMsg) {
  Temperature_meas = (inMsg.buf[2] << 24) + (inMsg.buf[3] << 16) + (inMsg.buf[4] << 8) + inMsg.buf[5];
  Temperature_meas = Temperature_meas/10;
}
void PowerMeasure(const CAN_message_t &inMsg) {
  Power_meas = (inMsg.buf[2] << 24) + (inMsg.buf[3] << 16) + (inMsg.buf[4] << 8) + inMsg.buf[5];
}
void BatteryStateMB(const CAN_message_t &inMsg) {
  Battery_state = (inMsg.buf[2] << 24) + (inMsg.buf[3] << 16) + (inMsg.buf[4] << 8) + inMsg.buf[5];
}
void BatterySOCPercentMB(const CAN_message_t &inMsg) {
  Battery_SOC_percent = inMsg.buf[1];
}
void BatteryVoltageMB(const CAN_message_t &inMsg) {
  Battery_Voltage = (inMsg.buf[1] << 24) + (inMsg.buf[2] << 16) + (inMsg.buf[3] << 8) + inMsg.buf[4];
  
}
void ChargerFlagsMB(const CAN_message_t &inMsg) {
  Charger_flags = inMsg.buf[4];
}
void MotorControllerMB(const CAN_message_t &inMsg) {
  Motor_Torqe = inMsg.buf[0];
  Motor_On = inMsg.buf[1];
  Motor_Voltage = (inMsg.buf[2] << 8) + inMsg.buf[3];
}
void Interrupt_Routine(){
  Status_Print();
}


void Send_Tourqe() {
  if(abs(Motor_Voltage - (Voltage_meas1/1000)) > 50)  
  {
    torqe_msg.buf[0] = 0;
    voltage_implausibility = 1;  
  }
  else
  {
    torqe_msg.buf[0] = Throttle;
    voltage_implausibility = 0;
  }
  torqe_msg.id = 0x81;
  torqe_msg.len = 1;
  torqe_msg.flags.extended = 0;
  torqe_msg.flags.remote   = 0;
  torqe_msg.flags.overrun  = 0;
  torqe_msg.flags.reserved = 0;
  
  Can1.write(torqe_msg);    //CANBus write command
}
