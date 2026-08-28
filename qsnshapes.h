//last edit 08.02.2026 last
//09.03.2026 - change to SNIRv2

#ifndef QSNSHAPES_H
#define QSNSHAPES_H

#define SLT_CAUTION 0
#define SLT_WARNING 1
#define SLT_INFORMATION 2
#define SLT_NOTE 3
#define SLT_DEBUG 5

#define BUSSERV_CONNECT_CHANGE 0
#define BUSSERV_CONNECT_CHANGE_connect 0
#define BUSSERV_CONNECT_CHANGE_disconnect 1
#define BUSSERV_ADAPTER_STATE 1
#define BUSSERV_ADAPTER_STATE_connected 0
#define BUSSERV_ADAPTER_STATE_disconnected 1
#define BUSSERV_TIME_SEC_EVENT 2
#define BUSSERV_MESSAGE 3
#define BUSSERV_MODULE_INIT 4
#define BUSSERV_MODULE_STATE_CHANGE 5
#define BUSSERV_MODULE_STATE_CHANGE_day 1
#define BUSSERV_MODULE_STATE_CHANGE_night 2
#define BUSSERV_MODULE_STATE_CHANGE_absence 3
#define BUSSERV_MODULE_STATE_CHANGE_alert 8
#define BUSSERV_MODULE_STATE_CHANGE_alarm 9
#define BUSSERV_ADDR_CHANGE_BEGIN 6
#define BUSSERV_ADDR_CHANGED 7
#define BUSSERV_ALERT 8
#define BUSSERV_ALERT_cancel 0
#define BUSSERV_ALARM 9
#define BUSSERV_ALARM_cancel 0
#define BUSSERV_ALARM_arming 1
#define BUSSERV_ALARM_arming_quietly 2
#define BUSSERV_ALARM_disarming 3
#define BUSSERV_ALARM_alarm 4
#define BUSSERV_ACCAUNT_CHANGED 10
#define BUSSERV_MGTT 11
#define BUSSERV_MGTT_reception_sn 0
#define BUSSERV_MGTT_request_sn 1
#define BUSSERV_MGTT_reception_raw 2
#define BUSSERV_MGTT_publication_sn 3
#define BUSSERV_MGTT_publication_raw 4
#define BUSSERV_LOG 12
#define BUSSERV_LOG_LEVEL_caution 0
#define BUSSERV_LOG_LEVEL_warning 1
#define BUSSERV_LOG_LEVEL_information 2
#define BUSSERV_LOG_LEVEL_note 3
#define BUSSERV_LOG_CATEGORY_core 1
#define BUSSERV_LOG_CATEGORY_adapter 2
#define BUSSERV_LOG_CATEGORY_module 4
#define BUSSERV_LOG_CATEGORY_network 8
#define BUSSERV_LOG_CATEGORY_bus 16
#define BUSSERV_LOG_CATEGORY_devices 32
#define BUSSERV_DB 13
#define BUSSERV_CONFIGUPDATE 14
#define BUSSERV_CONFIGUPDATE_BEGIN 0
#define BUSSERV_CONFIGUPDATE_END 1
#define BUSSERV_SNIRF 15

//signal data type indexes (0-63, see QSNTypeLabel)
#define QSN_TYPE_SIGNAL 0
#define QSN_TYPE_STATE 1
#define QSN_TYPE_CO2 2
#define QSN_TYPE_AIR_VELOCITY 3
#define QSN_TYPE_WATER_CONSUMPTION 4
#define QSN_TYPE_UINT8 5
#define QSN_TYPE_INT8 6
#define QSN_TYPE_UINT16 7
#define QSN_TYPE_INT16 8
#define QSN_TYPE_TEMP 9
#define QSN_TYPE_POWER 10
#define QSN_TYPE_CURRENT 11
#define QSN_TYPE_VOLTAGE 12
#define QSN_TYPE_DATETIME 13
#define QSN_TYPE_ALERT 14
#define QSN_TYPE_ERROR 15
#define QSN_TYPE_TEXT 16
#define QSN_TYPE_DATE 17
#define QSN_TYPE_TIME 18
#define QSN_TYPE_ENERGY 19
#define QSN_TYPE_HUMIDITY 20
#define QSN_TYPE_PRESSURE 21
#define QSN_TYPE_LEVEL 22
#define QSN_TYPE_DISTANCE 23
#define QSN_TYPE_ANGLE 24
#define QSN_TYPE_ILLUMINATION 25
#define QSN_TYPE_UINT32 26
#define QSN_TYPE_INT32 27
#define QSN_TYPE_CAPACITY 28
#define QSN_TYPE_WEIGHT 29
#define QSN_TYPE_HEX 30
#define QSN_TYPE_SECURITY 31
#define QSN_TYPE_SNIR 32
#define QSN_TYPE_EMUID 33
#define QSN_TYPE_RAINFALL 34
#define QSN_TYPE_TURNOVERS 35
#define QSN_TYPE_WEATHER 36
#define QSN_TYPE_DUST 37
#define QSN_TYPE_AQI 38
#define QSN_TYPE_RM 39
#define QSN_TYPE_FLOAT32 40
#define QSN_TYPE_GPS 41
#define QSN_TYPE_BATTERY 42
#define QSN_TYPE_RSSI 43
#define QSN_TYPE_FREQUENCY 44
#define QSN_TYPE_NOISE_LEVEL 45
#define QSN_TYPE_BITMASK 46
#define QSN_TYPE_COUNTER32 47
#define QSN_TYPE_ALTITUDE 48
#define QSN_TYPE_POWER_FACTOR 49
#define QSN_TYPE_SOIL_MOISTURE 50
#define QSN_TYPE_RADIATION 51
#define QSN_TYPE_PH_LEVEL 52
#define QSN_TYPE_FLOW_RATE 53
#define QSN_TYPE_VIBRATION 54
#define QSN_TYPE_TORQUE 55
#define QSN_TYPE_RUN_TIME 56
#define QSN_TYPE_MAC 57
#define QSN_TYPE_IP 58
#define QSN_TYPE_UPTIME 59
#define QSN_TYPE_FIRMWARE 60
#define QSN_TYPE_WIND_DIRECTION 61
#define QSN_TYPE_CONDUCTIVITY 62
#define QSN_TYPE_VARIANT 63

//extended data types 40-62 (payload size, scaling factor, JSON name)
// 40 FLOAT32       4 bytes (+postfix 5)      IEEE754              "float32"
// 41 GPS           6 bytes (int24 lat + int24 lon, deg*10000)     "gps"
// 42 BATTERY       1 byte (+voltage 3, mV)   0-100%              "battery"
// 43 RSSI          1 byte (+LQI 2)           -127..0 dBm         "rssi"
// 44 FREQUENCY     2 bytes (+IDP 4)          Hz*100              "frequency"
// 45 NOISE_LEVEL   2 bytes (+IDP 4)          dB*10               "noise"
// 46 BITMASK       1 byte (+IDP 3)           bits 0-7            "bitmask"
// 47 COUNTER32     4 bytes (+IDP 7)          0..4294967295       "counter"
// 48 ALTITUDE      3 bytes (+IDP 6)          m*10 (signed)       "altitude"
// 49 POWER_FACTOR  2 bytes (+IDP 4)          cosφ*1000           "powerFactor"
// 50 SOIL_MOISTURE 2 bytes (+IDP 4)          %*10                "soilMoisture"
// 51 RADIATION     4 bytes (+IDP 7)          µSv/h*10000         "radiation"
// 52 PH_LEVEL      2 bytes (+IDP 4)          pH*10               "ph"
// 53 FLOW_RATE     4 bytes (+IDP 7)          ml/min              "flowRate"
// 54 VIBRATION     4 bytes (+IDP 7)          mm/s*1000 (signed)  "vibration"
// 55 TORQUE        4 bytes (+IDP 7)          N·m*1000 (signed)   "torque"
// 56 RUN_TIME      4 bytes (+IDP 7)          hours               "runTime"
// 57 MAC           6 bytes (EUI-48)          "00:1A:2B:3C:4D:5E" "mac"
// 58 IP            4 bytes (IPv4)            "192.168.1.100"     "ip"
// 59 UPTIME        4 bytes (+IDP 7)          seconds             "uptime"
// 60 FIRMWARE      4 bytes (MAJOR.MINOR.PATCH.BUILD)             "firmware"
// 61 WIND_DIRECTION 2 bytes (+IDP 4)         deg*10              "windDirection"
// 62 CONDUCTIVITY  4 bytes (+IDP 7)          µS/cm*100           "conductivity"

#include <QObject>
#include <QVector>
#include <QStringList>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QPointF>
#include <qdebug.h>

extern bool QSNCelsius;
extern bool QSNMillimetersOfMercury;

struct QSNMessage
{
    quint8 Command;
    quint16 Address;
    quint16 Sender;
    QByteArray Data;
};

struct QSNSignal
{
    quint16 Signal;
    QByteArray Data;
};

struct QSNContainer
{
    enum type {signal, message, service, information, authorization, registration};
    type role;
    quint8 Command;
    quint16 Signal;
    quint16 Address;
    quint16 Sender;
    QByteArray Data;
    QString info;
};

struct QSNPower
{
    qint32 power;
    quint16 seconds;
    quint16 idp;
};

struct QSNNotification
{
    quint8 notificationType;
    quint16 idp;
    quint8 notificationSource;
};

struct QSNError
{
    quint32 errorIndex;
    quint16 idp;
    quint16 deviceType;
};

struct QSNDefaultChannel
{
    quint16 defaultSignal;
    QString defaultName;
    QString defaultNote;
    quint8 defaultSignalType;
};

struct QSNSNIR
{
    quint8 address;
    quint8 parameter;
    quint8 battery;
    quint8 retry;
    QByteArray data;
};

struct QSNRM
{
    quint8 type;
    QByteArray data;
};

struct QSNEMUID
{
    quint16 family;
    quint16 number;
};

// description of the types
QString QSNTypeLabel(quint8 index);
QString QSNTypeNote(quint8 index);
QString QSNTypeLatianName(quint8 index);
quint8 QSNLatianNameToType(QString type);
int QSNTypeIconIndex(quint8 index);
//QString QSNTypePfAlt(int multiplier, QString h, QString n, QString l);
QString QSNScalingNumToString(quint8 index, qreal num);

quint8 QSNScalingIndex(quint8 type, QVariant vol);
quint8 QSNPrecisionFromScaing(quint8 type, quint8 scaling);
QByteArray QSNVariantToRAW(QVariant vol, quint8 type);
QVariant QSNRAWToVariant(QByteArray *data);
QString QSNRAWtoScaledVolString(QByteArray *data, quint8 scale, QString text = QString());
QString QSNVariantToScaledString(QVariant vol, quint8 scale, quint8 type);
QString QSNVariantToScaledNumberString(QVariant vol, quint8 scale, quint8 type);
QString QSNRAWtoScaledVolume(QByteArray *data, QString text = QString());
QString QSNScaledPostFix(quint8 type, quint8 scale = 0);

//QString QSNScaledPostFix(quint8 index, int multiplier = 0);
QString QSNTypePrefix(quint8 index);
int QSNTypeDataSize(quint8 index);
QPointF QSNTypeRange(quint8 index);
int QSNAutoIcon(int iconNum, int dataType);

bool QSNTypeCompatibility(quint8 type1, bool input1, quint8 type2, bool input2);
QString QSNTypeRAWtoString(QByteArray *Data, QString text = QString());
quint8 QSNRAWtoPostFixIndex(QByteArray *data, quint8 index);
QString QSNRAWtoPostFix(QByteArray *data, quint8 index);
QString QSNRAWtoNotice(QByteArray *data);
QString QSNBoolTypetoLabel(bool state, quint8 index);
QString QSNTypeRAWtoLabel(QByteArray *Data);
QString QSNTypeRAWtoSubIndex(QByteArray *Data, quint8 index, int addr);
qreal QSNRAWtoReal(QByteArray *data, quint8 index, int addr);
quint16 QSNTypeRAWtoIDP(QByteArray *data);
quint16 QSNIDPtoDevAddr(quint16 idp);

// conversion types

qreal QSNCelsiusToFahrenheit(qreal celsius);
qreal QSNFahrenheitToCelsius(qreal fahrenheit);
qreal QSNFromCelsius(qreal celsius);
qreal QSNToCelsius(qreal value);
qreal QSNToMillimetersOfMercury(quint32 value);
QStringList QSNAlertTypeList();
QString QSNAlertTypeLabel(quint8 index);
QString QSNAlertTypeLabelSource(quint8 index);
QString QSNSecurityAlertTypeLabel(quint8 index);
QString QSNWeatherTypeLabel(quint8 index);
QString QSNDustTypeSize(quint8 index);
QString QSNRMType(quint8 index);

bool QSNRAWtoBit(QByteArray *Data, int addr, int bit);
void QSNBitToRAW(QByteArray *Data, int addr, int bit, bool state);
bool QSNRAWtoBool(QByteArray *Data, int addr);
void QSNBoolToRAW(QByteArray *Data, int addr, bool state);
quint8 QSNRAWtoByte(QByteArray *Data, int addr);
void QSNByteToRAW(QByteArray *Data, int addr, quint8 value);
qint8 QSNRAWtoInt8(QByteArray *Data, int addr);
void QSNInt8ToRAW(QByteArray *Data, int addr, qint8 value);
quint16 QSNRAWtoUInt16(QByteArray *Data, int addr);
void QSNUInt16ToRAW(QByteArray *Data, int addr, quint16 value);
qint16 QSNRAWtoInt16(QByteArray *Data, int addr);
void QSNInt16ToRAW(QByteArray *Data, int addr, qint16 value);
qint32 QSNRAWtoInt32(QByteArray *Data, int addr);
void QSNInt32ToRAW(QByteArray *Data, int addr, qint32 value);
qint32 QSNRAWtoInt24(QByteArray *Data, int addr);
void QSNInt24ToRAW(QByteArray *Data, int addr, qint32 value);
qint64 QSNRAWtoInt64(QByteArray *Data, int addr);
void QSNInt64ToRAW(QByteArray *Data, int addr, qint64 value);
quint32 QSNRAWtoUInt32(QByteArray *Data, int addr);
void QSNUInt32ToRAW(QByteArray *Data, int addr, quint32 value);
quint64 QSNRAWtoUInt64(QByteArray *Data, int addr);
void QSNUInt64ToRAW(QByteArray *Data, int addr, quint64 value);
qreal QSNRAWtoTemperature(QByteArray *Data, int addr);
void QSNTemperatureToRAW(QByteArray *Data, int addr, qreal value);
qreal QSNRAWtoHumidity(QByteArray *data, int addr);
void QSNHumidityToRAW(QByteArray *data, int addr, qreal value);
qreal QSNRAWtoFixedPoint(QByteArray *Data, int addr, quint8 point);
void QSNFixedPointToRAW(QByteArray *Data, int addr, qreal value, quint8 point);
QSNPower QSNRAWtoPower(QByteArray *Data, int addr);
void QSNPowerToRAW(QByteArray *Data, int addr, QSNPower power);
QDateTime QSNRAWtoDateTime(QByteArray *Data, int addr);
void QSNDateTimeToRAW(QByteArray *Data, int addr, QDateTime dateTime);
QSNNotification QSNRAWtoNotification(QByteArray *Data, int addr);
void QSNNotificationToRAW(QByteArray *Data, int addr, QSNNotification notification);
QSNError QSNRAWtoError(QByteArray *Data, int addr);
void QSNErrorToRAW(QByteArray *Data, int addr, QSNError error);
QString QSNRAWtoASCII(QByteArray *Data, int addr, int textLength);
void QSNASCIIToRAW(QByteArray *Data, int addr, QString text);
QString QSNRAWtoString(QByteArray *Data, QString text, int addr);
QString QSNRAWtoUTF8(QByteArray *Data, int addr);
void QSNUTF8ToRAW(QByteArray *Data, int addr, QString text);
QDate QSNRAWtoDate(QByteArray *Data, int addr);
void QSNDateToRAW(QByteArray *Data, int addr, QDate date);
QTime QSNRAWtoTime(QByteArray *Data, int addr);
void QSNTimeToRAW(QByteArray *Data, int addr, QTime time);
QString QSNRAWtoHEXcode(QByteArray *Data, int addr);
void QSNHEXcodeToRAW(QByteArray *Data, int addr, QString HEXstring);
float QSNRAWtoFloat(QByteArray *Data, int addr);
void QSNFloatToRAW(QByteArray *Data, int addr, float value);
QSNSNIR QSNRAWtoSNIR(QByteArray *Data, int addr);
void QSNSNIRToRAW(QByteArray *Data, int addr, QSNSNIR snir);
QByteArray QSNRAWtoRMcode(QByteArray *Data, int addr);
void QSNRMcodeToRAW(QByteArray *Data, int addr, QByteArray code);
QSNEMUID QSNRAWtoEmUID(QByteArray *Data, int addr);
void QSNEmUIDToRAW(QByteArray *Data, int addr, QSNEMUID uid);
QString QSNRAWtoVersion(QByteArray *Data, int addr);
void QSNVersionToRAW(QByteArray *Data, int addr, QString version);

//containers
QSNContainer newContainer();
void containerToStream(QSNContainer *container, QDataStream *stream);
QSNContainer containerFromStream(QDataStream *stream);
bool checkingContainerForRestrictions(QSNContainer *container);
void dataEncript(QByteArray *data, int start, QString key);
bool dataDecript(QByteArray *data, int start, QString key);
QSNContainer QSNLogToContainer(quint16 level, QString message, quint8 category, QString filename, bool isDebug = false);

//channels
QSNDefaultChannel QSNDefChanFromType(quint8 type);
QStringList QSNDefChanListLabels();

// utils
QDir QSNHomeDir();
QDir QSNHomePath(QString path);
QDir QSNHomeSubPath(QString path, QString sub);
QString QSNNormalizationFileName(QString name);
int QSNmemorySizeToBlockCount(int memorySize);
QString QSNAngleToString(qint16 minutes);
bool QSNCompareVersion(QString ver1, QString ver2);
QDateTime QSNStrToDateTime(QString strDT);
QString QSNStrToDateTime(QDateTime DT);
QString QSNDayWeekAbbreviated(int day);
QString QSNDaysWeekToLine(quint8 week);
QString QSNNameNormalization(QString name);
QString QSNBoolToText(bool state);
bool QSNTextToBool(QString state, bool *ok=Q_NULLPTR);
QString QSNBoolToYesNo(bool state);
QString QSNDecodeText(QString text);
QString QSNCleanIP(QString address);
QString QSNAllocateNumberFromString(QString str);
QString QSNUpTime(QDateTime begin);
QString QSNNameToFileName(QString str);
QString QSNWarningLevelToText(quint8 level);
QString QSNWarningLevelToLabel(quint8 level);
quint8 QSNWarningLevelFromText(QString text);
QString QSNGetWarninMSGNote(QString msg);
QString QSNGetWarninMSGBody(QString msg);
QString QSNEscapingHTMLCharacters(QString text);
QString QSNGetSerialErrorName(int error);
QString QSNContainerToLogText(QSNContainer container, int directionIn);
QString QSNMessageTypeToName(quint8 type);

bool QSNJsonToRAW(QByteArray *data, QString json, quint16 altIDP = 0);
bool QSNParseJSON(QString json, QMap<QString, QString> *options);
bool QSNJsonOptionsToRAW(QByteArray *data, QMap<QString, QString> *options, quint16 altIDP = 0);
void QSNStringToRAW(QByteArray *data, quint8 type, qreal multiplier, QString value1, QString value2, QString value3 = QString());
quint8 QSNBoolTypeStringToType(bool state, QString type);
QString QSNRAWToJSON(QByteArray *Data);
QString QSNRAWToJSONString(QByteArray *Data);
QString QSNRAWToValueString(QByteArray *Data, qreal multiplier);
bool QSNMQTTTopicFilterCompare(QString topic, QString filter);
QStringList QSNMQTTTopicFromString(QString topic);
QByteArray QSNMQTTTopicMsgToRAW(QString msg, quint8 valueType, QString jsonItem, qreal multiplier);
long QSNPOW(long int x, unsigned int n);
quint8 QSNSNIRBatToPercent(quint8 raw);


QString QSNUInt32ToHEXString(quint32 vol);
quint32 QSNHEXStringToUInt32(QString str);
QDateTime QSNNULLDateTime();
QString QSNDateTimeToString(QDateTime dtime);
QString QSNToTimeString(QDateTime dtime);




#endif // QSNSHAPES_H
