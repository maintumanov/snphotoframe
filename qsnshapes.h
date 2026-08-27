//last edit 24.09.2019

#ifndef QSNSHAPES_H
#define QSNSHAPES_H

#define SLT_CAUTION 0
#define SLT_WARNING 1
#define SLT_INFORMATION 2
#define SLT_NOTE 3
#define SLT_DEBUG 5
#define SLT_MASK_NETWORK 32

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
};

struct QSNNotification
{
    quint8 notificationType;
    quint16 deviceAddress;
};

struct QSNError
{
    quint32 errorType;
    quint16 deviceAddress;
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
    QByteArray Data;
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
int QSNTypeIconIndex(quint8 index);
QString QSNTypePfAlt(int multiplier, QString h, QString n, QString l);
QString QSNScalingNumToString(quint8 index, qreal num);
QString QSNTypePostFix(quint8 index, int multiplier = 0);
QString QSNTypePreFix(quint8 index);
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
QString QSNSecurityAlertTypeLabel(quint8 index);

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
QSNSNIR QSNRAWtoSNIR(QByteArray *Data, int addr);
void QSNSNIRToRAW(QByteArray *Data, int addr, QSNSNIR snir);
QSNEMUID QSNRAWtoEmUID(QByteArray *Data, int addr);
void QSNEmUIDToRAW(QByteArray *Data, int addr, QSNEMUID uid);
QString QSNRAWtoVersion(QByteArray *Data, int addr);
void QSNVersionToRAW(QByteArray *Data, int addr, QString version);

//containers
QSNContainer newContainer();
void containerToStream(QSNContainer *container, QDataStream *stream);
QSNContainer containerFromStream(QDataStream *stream);
void dataEncript(QByteArray *data, int start, QString key);
bool dataDecript(QByteArray *data, int start, QString key);

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
bool QSNTextToBool(QString state);
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
QString QSNContainerToLogText(QSNContainer container, bool directionIn);
QString QSNMessageTypeToName(quint8 type);


#endif // QSNSHAPES_H
