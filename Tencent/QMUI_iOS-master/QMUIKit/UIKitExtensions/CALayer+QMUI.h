/**
 * Tencent is pleased to support the open source community by making QMUI_iOS available.
 * Copyright (C) 2016-2021 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
 */

//
//  CALayer+QMUI.h
//  qmui
//
//  Created by QMUI Team on 16/8/12.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_OPTIONS (NSUInteger, QMUICornerMask) {
    QMUILayerMinXMinYCorner = 1U << 0,
    QMUILayerMaxXMinYCorner = 1U << 1,
    QMUILayerMinXMaxYCorner = 1U << 2,
    QMUILayerMaxXMaxYCorner = 1U << 3,
    QMUILayerAllCorner = QMUILayerMinXMinYCorner|QMUILayerMaxXMinYCorner|QMUILayerMinXMaxYCorner|QMUILayerMaxXMaxYCorner,
};

@interface CALayer (QMUI)

/// 是否为某个 UIView 自带的 layer
@property(nonatomic, assign, readonly) BOOL qmui_isRootLayerOfView;

/// 暂停/恢复当前 layer 上的所有动画
@property(nonatomic, assign) BOOL qmui_pause;

/**
 *  设置四个角是否支持圆角的，iOS11 及以上会调用系统的接口，否则 QMUI 额外实现
 *  @warning 如果对应的 layer 有圆角，则请使用 QMUIBorder，否则系统的 border 会被 clip 掉
 *  @warning 使用 qmui 方法，则超出 layer 范围内的内容都会被 clip 掉，系统的则不会
 *  @warning 如果使用这个接口设置圆角，那么需要获取圆角的值需要用 qmui_originCornerRadius，否则 iOS 11 以下获取到的都是 0
 */
@property(nonatomic, assign) QMUICornerMask qmui_maskedCorners DEPRECATED_MSG_ATTRIBUTE("请使用系统的 CALayer.maskedCorners，QMUI 4.4.0 开始不再支持 iOS 10，该属性无意义了，后续会删除。");

/// iOS11 以下 layer 自身的 cornerRadius 一直都是 0，圆角的是通过 mask 做的，qmui_originCornerRadius 保存了当前的圆角
@property(nonatomic, assign, readonly) CGFloat qmui_originCornerRadius;

/**
 支持直接用一个 NSShadow 来设置各种 shadow 样式（其实就是把分散的多个 shadowXxx 接口合并为一个）。不保证样式的锁定（也即如果后续用独立的 shadowXxx 接口修改了样式则会被覆盖）。
 @note 当使用这个接口时，shadowOpacity 会强制设置为1，阴影的半透明请通过修改 NSShadow.shadowColor 颜色里的 alpha 来控制。仅当之前已经设置过 qmui_shadow 的情况下，才可以通过 qmui_shadow = nil 来去除阴影。
 */
@property(nonatomic, strong, nullable) NSShadow *qmui_shadow;

/**
 只有当前 layer 里被返回的路径包裹住的内容才能被看到，路径之外的区域被裁剪掉。
 该 block 会在 layer 大小发生变化时被调用，所以请根据 aLayer.bounds 计算实时的路径。
 */
@property(nonatomic, copy, nullable) UIBezierPath * (^qmui_maskPathBlock)(__kindof CALayer *aLayer);

/**
 与 qmui_maskPathBlock 相反，返回的路径会将当前 layer 的内容裁切掉，例如假设返回一个 layer 中间的矩形路径，则这个矩形会被挖空，其他区域正常显示。
 该 block 会在 layer 大小发生变化时被调用，所以请根据 aLayer.bounds 计算实时的路径。
 */
@property(nonatomic, copy, nullable) UIBezierPath * (^qmui_evenOddMaskPathBlock)(__kindof CALayer *aLayer);

/// 获取指定 name 值的 layer，包括 self 和 self.sublayers，会一直往 sublayers 查找直到找到目标 layer。
- (nullable __kindof CALayer *)qmui_layerWithName:(NSString *)name;

/**
 *  把某个 sublayer 移动到当前所有 sublayers 的最后面
 *  @param sublayer 要被移动的 layer
 *  @warning 要被移动的 sublayer 必须已经添加到当前 layer 上
 */
- (void)qmui_sendSublayerToBack:(CALayer *)sublayer;

/**
 *  把某个 sublayer 移动到当前所有 sublayers 的最前面
 *  @param sublayer 要被移动的layer
 *  @warning 要被移动的 sublayer 必须已经添加到当前 layer 上
 */
- (void)qmui_bringSublayerToFront:(CALayer *)sublayer;

/**
 * 移除 CALayer（包括 CAShapeLayer 和 CAGradientLayer）所有支持动画的属性的默认动画，方便需要一个不带动画的 layer 时使用。
 */
- (void)qmui_removeDefaultAnimations;

/**
 * 对 CALayer 执行一些操作，不以动画的形式展示过程（默认情况下修改 CALayer 的属性都会以动画形式展示出来）。
 * @param actionsWithoutAnimation 要执行的操作，可以在里面修改 layer 的属性，例如 frame、backgroundColor 等。
 * @note 如果该 layer 的任何属性修改都不需要动画，也可使用 qmui_removeDefaultAnimations。
 */
+ (void)qmui_performWithoutAnimation:(void (NS_NOESCAPE ^)(void))actionsWithoutAnimation;

/**
 * 生成虚线的方法，注意返回的是 CAShapeLayer
 * @param lineLength   每一段的线宽
 * @param lineSpacing  线之间的间隔
 * @param lineWidth    线的宽度
 * @param lineColor    线的颜色
 * @param isHorizontal 是否横向，因为画虚线的缘故，需要指定横向或纵向，横向是 YES，纵向是 NO。
 * 注意：暂不支持 dashPhase 和 dashPattens 数组设置，因为这些都定制性太强，如果用到则自己调用系统方法即可。
 */
+ (CAShapeLayer *)qmui_separatorDashLayerWithLineLength:(NSInteger)lineLength
                                            lineSpacing:(NSInteger)lineSpacing
                                              lineWidth:(CGFloat)lineWidth
                                              lineColor:(CGColorRef)lineColor
                                           isHorizontal:(BOOL)isHorizontal;

/**
 
 * 产生一个通用分隔虚线的 layer，高度为 PixelOne，线宽为 2，线距为 2，默认会移除动画，并且背景色用 UIColorSeparator，注意返回的是 CAShapeLayer。
 
 * 其中，InHorizon 是横向；InVertical 是纵向。
 
 */
+ (CAShapeLayer *)qmui_separatorDashLayerInHorizontal;

+ (CAShapeLayer *)qmui_separatorDashLayerInVertical;

/**
 * 产生一个适用于做通用分隔线的 layer，高度为 PixelOne，默认会移除动画，并且背景色用 UIColorSeparator
 */
+ (CALayer *)qmui_separatorLayer;

/**
 * 产生一个适用于做列表分隔线的 layer，高度为 PixelOne，默认会移除动画，并且背景色用 TableViewSeparatorColor
 */
+ (CALayer *)qmui_separatorLayerForTableView;

@end

@interface CALayer (QMUI_DynamicColor)

/// 如果 layer 的 backgroundColor、borderColor、shadowColor 是使用 dynamic color（UIDynamicProviderColor、QMUIThemeColor 等）生成的，则调用这个方法可以重新设置一遍这些属性，从而更新颜色
/// iOS 13 系统设置里的界面样式变化（Dark Mode），以及 QMUIThemeManager 触发的主题变化，都会自动调用 layer 的这个方法，业务无需关心。
- (void)qmui_setNeedsUpdateDynamicStyle NS_REQUIRES_SUPER;
@end

NS_ASSUME_NONNULL_END
