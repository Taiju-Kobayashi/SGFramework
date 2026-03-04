# エンジンコア再設計案（柔軟性重視）

## 1. 現状の課題（コード観点）

- `Manager` が `Renderer` / `Input` / `Audio` / `Scene` 初期化とループ制御を一手に持っており、責務が集中している。
- `Scene` と `GameObject` が生ポインタ中心で、ライフサイクル管理（生成/破棄）が分散している。
- `GameObject::Destroy()` が `delete this` を行うため、呼び出し順と参照安全性への依存が強い。
- `Scene::AddGameObject<T>()` が `new` + `Init()` を直接行うため、生成戦略差し替えやテストダブル注入が難しい。
- システムアクセスがグローバル/シングルトン前提で、実行環境（本番/テスト/ヘッドレス）の切り替えがしづらい。

## 2. 再設計の基本方針

### A. 「更新の主体」をオブジェクトからシステムへ寄せる

- `GameObject` 主体の更新から、`System` 主体（TransformSystem, RenderSystem, InputSystem など）へ寄せる。
- エンティティは ID のみ、データはコンポーネント配列（SoA寄り）で管理し、検索・並列化しやすくする。

### B. 「依存の注入」を標準化する

- `EngineContext`（サービスロケータではなく明示依存）を導入し、
  - `IRenderDevice`
  - `IInputService`
  - `IAudioService`
  - `IAssetRepository`
  などのインターフェイスを集約。
- Scene/System は抽象インターフェイスに依存し、実装差し替え可能にする。

### C. ライフサイクルを2段階化する

- `requestDestroy(entity)` のような遅延破棄キューを導入し、フレーム終端で一括適用。
- `Init/Uninit` 直呼びより、`OnEnter/OnExit`（Scene）・`OnAttach/OnDetach`（System）へ整理。

## 3. 目標アーキテクチャ（最小構成）

- `Engine`
  - `Run()`：固定更新 + 可変描画のメインループ
  - `SceneDirector`：シーン遷移オーケストレーション
  - `World`：Entity/Component ストレージ
  - `SystemScheduler`：更新順序・依存解決
  - `EngineContext`：外部サービス依存

### インターフェイス例

```cpp
struct ISystem {
  virtual ~ISystem() = default;
  virtual void configure(World&, EngineContext&) {}
  virtual void update(World&, EngineContext&, float dt) = 0;
};

struct IScene {
  virtual ~IScene() = default;
  virtual void onEnter(World&, EngineContext&) = 0;
  virtual void onExit(World&, EngineContext&) = 0;
};
```

## 4. 更新ループ設計（可変・固定の分離）

- 物理・ゲームロジック：固定ステップ（例: 60Hz）
- 入力収集：フレーム先頭で1回
- 描画：可能な限り毎フレーム
- これにより、現在の単一 `GameLoop()` より時間管理を明確化できる。

擬似コード:

```cpp
while (running) {
  pollPlatformEvents();
  input.beginFrame();

  accumulator += clock.delta();
  while (accumulator >= fixedDt) {
    scheduler.runPhase(UpdatePhase::Fixed, fixedDt);
    world.applyDeferredCommands();
    accumulator -= fixedDt;
  }

  scheduler.runPhase(UpdatePhase::Variable, clock.delta());
  renderer.beginFrame();
  scheduler.runPhase(UpdatePhase::Render, 0.0f);
  renderer.endFrame();
}
```

## 5. シーン管理の再設計

- 現状の非同期ロード方針は活かしつつ、`SceneDirector` に責務を集約。
- 遷移は `Requested -> Loading -> Activating -> Active` の状態機械で管理。
- ロード完了後の `onEnter` はメインスレッドで実行し、GPU/APIスレッド制約を守る。

## 6. データ駆動化（柔軟性の源泉）

- `Scene` を C++ 直書き中心から、JSON/YAML + Factory に段階移行。
- 例:
  - `prefab/stone.json`
  - `scene/title.scene.json`
- コードは「読み込んだ定義をどう実体化するか」に集中できる。

## 7. 段階的移行プラン

1. `delete this` を廃止し、遅延破棄キューへ置換。
2. `EngineContext` とインターフェイスを導入、`Manager` から直参照を段階剥離。
3. `SceneDirector` を導入し、シーン遷移処理を `Manager` から移管。
4. `SystemScheduler` を作成し、`Scene::ParentUpdate` の更新責務を移行。
5. 新規機能は ECS/システム側で実装し、旧方式との共存期間を設ける。

## 8. 期待できる効果

- 機能追加時の影響範囲縮小（シーン・描画・入力の分離）。
- テスト容易性向上（モック入力・ヘッドレス描画）。
- マルチスレッド化余地増加（システム単位で並列化）。
- メモリ安全性向上（所有権の明確化、破棄時機の統制）。

