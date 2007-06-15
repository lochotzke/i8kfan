#define I8kfanDaemonRegisteredName  @"com.PotatoSalad.I8kfanDaemonRegistered"

@protocol I8kfanProtocol
- (int)baseRpm;
- (void)setBaseRpm:(int)newBaseRpm;
- (float)lowerThreshold;
- (void)setLowerThreshold:(float)newLowerThreshold;
- (float)upperThreshold;
- (void)setUpperThreshold:(float)newUpperThreshold;
- (BOOL)fahrenheit;
- (void)setFahrenheit:(BOOL)newFahrenheit;
- (void)temperature:(float *)temperature leftFanRpm:(int *)leftFanRpm rightFanRpm:(int *)rightFanRpm;
@end
