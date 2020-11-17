# Material Editor ヘルプ

## はじめに
"Material Editor"は，モデルに設定されているマテリアルを編集するツールです。マテリアルの割り当てはDCCツール上で事前に設定しておいてください。  
まず，DCCツール上でモデル（およびメッシュ）の作成を行い，マテリアルの割り当てまで行い，"Material Editor"で読み込み対応しているアセットファイル形式でエクスポートを行ってください。対応しているファイル形式は次の通りです。

### 対応モデル形式
* fbx
* obj
* 3ds
* dae
* collada
* blend
* dxf
* gltf
* glb
* lwo
* mdl
* md2
* md3
* md5
* ms3d
* ply
* stl
* x

### 対応テクスチャ形式
* dds
* bmp
* jpg
* png
* tiff
* gif
* hdp

エクスポートされたファイルを読み込んだら，マテリアルの編集を行うことができます。マテリアルの定義は，HLSLファイルと，XMLファイルのセットで行うことができ，エディタ起動時plugin/materialフォルダに配置されているものが自動で読み込みされます。


## 画面表示について  
Material Editorの画面について説明します。  

/* TODO */


## マテリアル定義について  
Material Editorでは，マテリアル定義にXMLファイルを使用しています。マテリアル定義は, ユーザーが自由に定義を追加したり，拡張することが可能です。ここでは，ユーザーが拡張を行うためにマテリアル定義について説明します。

### マテリアル定義ファイル(*.xml)
マテリアル定義は，次の要素で構成されます。

* マテリアル名
* マテリアルシェーダファイルパス
* シャドウキャストフラグ
* シャドウレシーブフラグ
* ブレンドステート
* ラスタライザーステート
* 深度ステート
* マテリアルパラメータ

以下は，Material Editorに備え付けの標準マテリアルの定義例です。  

```xml
<?xml version="1.0" encoding="UTF-8"?>
<PluginMaterial name="標準" shader_path="../shader/StandardPS.hlsl">
    <shadow_cast editable="true" default="false"/>
    <shadow_receive editable="true" default="false"/>
    <blend_state editable="true" default="0"/>
    <rasterizer_state editable="true" default="0"/>
    <depth_state editable="true" default="1"/>
    <float2 label="UVスクロール" target="UVOffset" default_x="0" default_y="0" step="0.1" mini="0" maxi="0" />
    <float2 label="UVスケール" target="UVScale" default_x="1" default_y="1" step="0.1" mini="0" maxi="0" />
    <float label="UV回転" target="UVRotate" default="0" step="0.1" mini="-360.0" maxi="360.0" converter="1" />
    <float label="アルファテスト閾値" target="AlphaThreshold" default="0" step="0.01" mini="0" maxi="1" />
    <texture2D label="ベースカラー" target="BaseColor" default="0" />
    <texture2D label="ORM" target="ORM" default="4" />
    <texture2D label="法線マップ" target="Normal" default="3" />
    <texture2D label="エミッシブ" target="Emissive" default="2" />
</PluginMaterial>
```

#### PluginMaterialエレメント  
プラグインマテリアルを定義する要素です。

* name  
    マテリアル名を指定します.
* shader_path  
    マテリアルシェーダのファイルパスを指定します.  

#### shadow_castエレメント
シャドウを投影するかどうかを指定する要素です。

* editable  
    エディタ上で編集可能にする場合は true, 編集不可の非表示にする場合は false を指定します。
* default  
    デフォルト値を指定します。  

#### shadow_receiveエレメント
シャドウを受けるかどうかを指定する要素です。  

* editable
    エディタ上で編集可能にする場合は true, 編集不可の非表示にする場合は false を指定します。
* default  
    デフォルト値を指定します。  

#### blend_stateエレメント
ブレンドステートを定義する要素です。

* editable  
    エディタ上で編集可能にする場合は true, 編集不可の非表示にする場合は false を指定します。
* default  
    デフォルト値を指定します。  

ブレンドステートには次の値を設定することができます。
* 0 : 不透明
* 1 : アルファブレンド
* 2 : 加算
* 3 : 減算
* 4 : 事前乗算済みアルファブレンド
* 5 : 乗算
* 6 : スクリーン


#### rasterizer_stateエレメント
ラスタライザーステートを定義する要素です。

* editable  
    エディタ上で編集可能にする場合は true, 編集不可の非表示にする場合は false を指定します。
* default  
    デフォルト値を指定します。  

ラスタライザーステートには次の値を設定することができます。
* 0 : カリング無し(両面表示)
* 1 : 時計回りのポリゴンをカリング(背面表示)
* 2 : 反時計周りのポリゴンをカリング(前面表示)


#### depth_stateエレメント
深度ステートを定義する要素です。

* editable  
    エディタ上で編集可能にする場合は true, 編集不可の非表示にする場合は false を指定します。
* default  
    デフォルト値を指定します。  

深度ステートには次の値を設定することができます。
* 0 : 深度テスト無し，深度書き込み無し
* 1 : 深度テスト有り, 深度書き込み有り
* 2 : 深度テスト有り, 深度書き込み無し
* 3 : 深度テスト無し, 深度書き込み有り

#### boolエレメント
bool型を定義するマテリアルパラメータです。シェーダには0または1の値として設定されます。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* default  
    デフォルト値を指定します。  

#### intエレメント
符号付き整数を定義するマテリアルパラメータです。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* defualt  
    デフォルト値を指定します。  
* step  
    スライダーの刻み値を指定します。
* mini  
    有効な値の最小値を設定します。最小値を設けない場合は0を指定して下さい。
* maxi  
    有効な値の最大値を設定します。最大値を設けない場合は0を指定してください。

#### floatエレメント
単精度浮動小数を定義するマテリアルパラメータです。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* defualt  
    デフォルト値を指定します。  
* step  
    スライダーの刻み値を指定します。
* mini  
    有効な値の最小値を設定します。最小値を設けない場合は0を指定して下さい。
* maxi  
    有効な値の最大値を設定します。最大値を設けない場合は0を指定してください。
* converter  
    値を変換するコンバーターを指定します。変換無しで即値を渡す場合は0を，ラジアン単位に変換する場合は1，度単位に変換する場合は2を指定します。  

#### float2エレメント
2次元ベクトル定義するマテリアルパラメータです。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* defualt  
    デフォルト値を指定します。  
* defualt_x  
    X成分のデフォルト値を指定します。  
* defualt_y  
    Y成分のデフォルト値を指定します。  
* step  
    スライダーの刻み値を指定します。
* mini  
    有効な値の最小値を設定します。最小値を設けない場合は0を指定して下さい。
* maxi  
    有効な値の最大値を設定します。最大値を設けない場合は0を指定してください。
* converter  
    値を変換するコンバーターを指定します。変換無しで即値を渡す場合は0を，ラジアン単位に変換する場合は1，度単位に変換する場合は2を指定します。  

#### float3エレメント
3次元ベクトル定義するマテリアルパラメータです。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* defualt_x  
    X成分のデフォルト値を指定します。  
* defualt_y  
    Y成分のデフォルト値を指定します。  
* defualt_z  
    Z成分のデフォルト値を指定します。  
* step  
    スライダーの刻み値を指定します。
* mini  
    有効な値の最小値を設定します。最小値を設けない場合は0を指定して下さい。
* maxi  
    有効な値の最大値を設定します。最大値を設けない場合は0を指定してください。
* converter  
    値を変換するコンバーターを指定します。変換無しで即値を渡す場合は0を，ラジアン単位に変換する場合は1，度単位に変換する場合は2を指定します。  


#### float4エレメント
4次元ベクトル定義するマテリアルパラメータです。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* defualt_x  
    X成分のデフォルト値を指定します。  
* defualt_y  
    Y成分のデフォルト値を指定します。  
* defualt_z  
    Z成分のデフォルト値を指定します。  
* defualt_w  
    W成分のデフォルト値を指定します。  
* step  
    スライダーの刻み値を指定します。
* mini  
    有効な値の最小値を設定します。最小値を設けない場合は0を指定して下さい。
* maxi  
    有効な値の最大値を設定します。最大値を設けない場合は0を指定してください。
* converter  
    値を変換するコンバーターを指定します。変換無しで即値を渡す場合は0を，ラジアン単位に変換する場合は1，度単位に変換する場合は2を指定します。  


#### color3エレメント
3次元ベクトル定義するマテリアルパラメータです。float3エレメントと異なり，カラー表示専用のGUIが描画されます。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* default_r  
    R成分のデフォルト値を[0, 1]の範囲で設定します。  
* default_g  
    G成分のデフォルト値を[0, 1]の範囲で設定します。  
* default_b  
    B成分のデフォルト値を[0, 1]の範囲で設定します。  
* wheel  
    カラーホイールとしてGUIを表示する場合は true を指定，カラーピッカーとしてGUI表示する場合は false を指定します。  


#### color4エレメント
4次元ベクトル定義するマテリアルパラメータです。float4エレメントと異なり，カラー表示専用のGUIが描画されます。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* default_r  
    R成分のデフォルト値を[0, 1]の範囲で設定します。  
* default_g  
    G成分のデフォルト値を[0, 1]の範囲で設定します。  
* default_b  
    B成分のデフォルト値を[0, 1]の範囲で設定します。  
* default_a  
    A成分のデフォルト値を[0, 1]の範囲で設定します。  
* wheel  
    カラーホイールとしてGUIを表示する場合は true を指定，カラーピッカーとしてGUI表示する場合は false を指定します。  


#### bit32エレメント
32ビットのフラグを定義するマテリアルパラメータです。  

* label
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* default
    デフォルト値を指定します。  


#### texture2Dエレメント
2次元テクスチャを定義するマテリアルパラメータです。  

* label  
    エディタ上での表示名です。  
* target  
    設定先のシェーダ変数名を指定します。  
* default  
    デフォルトテクスチャを指定します。  

デフォルトテクスチャとして指定できる値は次の通りです。
* 0 : 白と灰色の市松模様（チェッカーボード）
* 1 : 白
* 2 : 黒
* 3 : 法線
* 4 : オクルージョン/ラフネス/メタリック
* 5 : ベロシティ


### マテリアルシェーダ(*.hlsl)
マテリアルの描画に使用するシェーダはHLSLファイルでShader Model 5.0 のピクセルシェーダを用意してください。また，Material Editorで使用するリソースの定義が，plugins/shader/Editor.hlsliにまとまっています。レジスタ10番まではエディタシステム予約扱いになっていますので，11番以降を使用するようにしてください。  
ピクセルシェーダでは，通常描画用のメインエントリーポイントのLightingPS()メソッドと，シャドウ描画用のメインエントリーポイントのShadowingPS()メソッドの2つを実装する必要があります。また，エディタの制限として，CbUserという名前が付いている定数バッファのみをエディタ上で更新する仕組みになっています。エディタ上で編集したいパラメータはCbUserの中に含めてください。  

以下は，Material Editorに備え付けのStandardPS.hlslの実装例です。

```hlsl

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Editor.hlsli"

/* ユーザー定義データはスロット番号11から開始してください. 10番まではシステム使用とします.*/

///////////////////////////////////////////////////////////////////////////////
// CbUser constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbUser : register(b11)
{
    float2 UVOffset         : packoffset(c0);
    float2 UVScale          : packoffset(c0.z);
    float  UVRotate         : packoffset(c1);
    float  AlphaThreshold   : packoffset(c1.y);
};

//-----------------------------------------------------------------------------
// Textures
//-----------------------------------------------------------------------------
Texture2D BaseColor : register(t11);
Texture2D ORM       : register(t12);
Texture2D Normal    : register(t13);
Texture2D Emissive  : register(t14);


//-----------------------------------------------------------------------------
//      ライティング処理です.
//-----------------------------------------------------------------------------
PSOutput LightingPS(const VSOutput input)
{
    PSOutput output = (PSOutput)0;

    // UVアニメーション.
    float2 uv = UVAnimation(GetTexCoord0(input), UVScale, UVOffset, UVRotate);

    // ベースカラー取得.
    float4 baseColor = BaseColor.Sample(AnisotropicWrap, uv);
    if (baseColor.a < AlphaThreshold)
    { discard; }

    // 法線／オクルージョン／ラフネス／メタルネスを取得.
    float3 normal = Normal.Sample(AnisotropicWrap, uv).xyz * 2.0f - 1.0f;
    float3 orm    = ORM.Sample(AnisotropicWrap, uv).rgb;

    // 従接線ベクトルを計算.
    float3 bitangent = GetBitangent(input);
    float3 N = FromTangentSpaceToWorld(normal, input.Tangent, bitangent, input.Normal);

    // ビュー空間の深度と位置座標を取得.
    float3 viewPos = GetViewPosition(input);

    // ワールド空間の位置を復元.
    float3 worldPos = ToWorldPosition(viewPos);

    // 視線ベクトルライトベクトルを算出.
    float3 V = normalize(CameraPos - worldPos);
    float3 L = normalize(SunLightDir);

    float3 kd = ToKd(baseColor.rgb, orm.z);
    float3 ks = ToKs(baseColor.rgb, orm.z);

    // ライティング.
    output.Color.rgb += EvaluateIBLIsotropy(N, V, kd, ks, orm.x, orm.y, orm.z);
    output.Color.rgb += EvaluateDirectLightIsotropicGGX(N, V, L, kd, ks, orm.y);
    output.Color.rgb += Emissive.Sample(AnisotropicWrap, uv).rgb;
    output.Color.a = 1.0f;

    output.NRM = EncodeNRM(N, orm.y, orm.z);

    return output;
}

//-----------------------------------------------------------------------------
//      シャドウイング処理です.
//-----------------------------------------------------------------------------
void ShadowingPS(const VSOutput input)
{
    // UVアニメーション.
    float2 uv = UVAnimation(GetTexCoord0(input), UVScale, UVOffset, UVRotate);

    // ベースカラー取得.
    float4 baseColor = BaseColor.Sample(AnisotropicWrap, uv);
    if (baseColor.a < AlphaThreshold)
    { discard; }
}
```

通常描画時は，ライティング結果をPSOutput.Colorに，法線／ラフネス／メタルネスの値をPSOutput.NRMに格納してください。
シャドウ描画時は，discard命令が必要なもののみ，ShadowingPS()メソッドを実装してください。  

## エクスポートプラグインについて
Material Editorではエクスポート処理をカスタマイズできるように，DLL内にあるExportMaterialという関数を呼び出す仕組みになっています。  
plugins/exporterフォルダ下にDLLを置くことで，エディタ上から呼び出し可能になります。どのDLLを選択するかについてはエクスポート設定からコンボボックスで選択できるようになっています。  


---
以上。