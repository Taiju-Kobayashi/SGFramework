## 概要 📖

`SGFramework` は、ゲームフレームワークの設計を学習する目的で作成されたものです。

このプロジェクトを通じて、モダンなC++を用いたゲームエンジンの基本的な構造、コンポーネントベースの設計思想、そしてゲームループやシーン管理といったコア機能の実装を探求します。

## このプロジェクトの学習目標 🎯

  * **フレームワークのアーキテクチャ理解:** ゲームの基本的な骨格となる構造をゼロから設計・実装する。
  * **コンポーネントベース設計の習得:** オブジェクトを部品（コンポーネント）の集合として捉え、柔軟で再利用性の高い設計を学ぶ。
  * **コア機能の実装:**
      * ゲームループ
      * シーン管理
      * 入力処理（キーボード、マウス）
      * シンプルなレンダリング
  * **C++のモダンな機能の活用:** スマートポインタ、ラムダ式、その他のC++17/20の機能を実践的に利用する。

## 主な機能 ✨

現在、以下の基本的な機能が実装されています。

  * **ウィンドウ管理:** アプリケーションのウィンドウ生成と管理。
  * **DirectX11ベースのレンダリング:** 2Dスプライトや基本的な図形の描画。
  * **入力システム:** キーボードとマウスからの入力をハンドリング。
  * **シーンマネージャー:** ゲームの各画面（タイトル、プレイ中、結果など）を管理する仕組み。
  * **基本的なコンポーネントシステム:** `Actor` と `Component` をベースとしたオブジェクト管理。

## 動作環境 💻

  * **OS:** Windows 10 / 11
  * **IDE:** Visual Studio 2022
  * **SDK:** Windows 11 SDK
  * **言語:** C++20

## 今後の課題 📝

学習の進捗に応じて、以下の機能の実装を目指しています。

  * [ ] 物理演算の導入
  * [ ] サウンドライブラリの統合
  * [ ] アニメーションシステムの構築
  * [ ] UIシステムの改善
  * [ ] より高度なシェーダーの活用

## ライセンス 📄

このプロジェクトは学習目的で作成されており、特定のライセンスは設定されていません。

-----
