
#include <gtest/gtest.h>

#include "sqlite_back.h"
#include "recorder.h"
#include "context.h"
#include "timer.h"
#include "sim_init.h"
#include "facility.h"
#include "resource_buff.h"

static const char* dbpath = "testsiminit.sqlite";

namespace cy = cyclus;

class Inver : public cy::Facility {
 public:
  Inver(cy::Context* ctx) : cy::Facility(ctx), val1(0) {}
  virtual ~Inver() {}

  virtual cy::Agent* Clone() {
    Inver* i = new Inver(context());
    i->InitFrom(this);
    return i;
  }

  virtual void InitFrom(Inver* a) {
    cy::Facility::InitFrom(a);
    val1 = a->val1;
  };

  virtual void InitFrom(cy::QueryableBackend* b) {
    cy::Facility::InitFrom(b);
    cy::QueryResult qr = b->Query("Info", NULL);
    val1 = qr.GetVal<int>("val1");
  };

  virtual void Snapshot(cy::DbInit di) {
    di.NewDatum("Info")
      ->AddVal("val1", val1)
      ->Record();
  };

  virtual void Build(cy::Agent* parent) {
    cy::Facility::Build(parent);

    cy::CompMap v;
    v[922350000] = 1;
    v[922380000] = 2;
    cy::Composition::Ptr c = cy::Composition::CreateFromMass(v);
    cy::Material::Ptr m1 = cy::Material::Create(this, 1, c);
    cy::Material::Ptr m2 = cy::Material::Create(this, 2, c);

    v[922380000] = 3;
    c = cy::Composition::CreateFromMass(v);
    cy::Material::Ptr m3 = cy::Material::Create(this, 3, c);

    buf1.Push(m1);
    buf2.Push(m2);
    buf2.Push(m3);
  };

  virtual void InitInv(cy::Inventories& inv) {
    buf1.PushAll(inv["buf1"]);
    buf2.PushAll(inv["buf2"]);
  };

  virtual cy::Inventories SnapshotInv() {
    cy::Inventories invs;
    invs["buf1"] = buf1.PopN(buf1.count());
    invs["buf2"] = buf2.PopN(buf2.count());
    return invs;
  }
  virtual void Tick(int t) { };
  virtual void Tock(int t) { };

  cy::ResourceBuff buf1;
  cy::ResourceBuff buf2;
  int val1;
};

class SimInitTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    rec = new cy::Recorder();
    b = new cy::SqliteBack(dbpath);
    rec->RegisterBackend(b);
    ctx = new cy::Context(&ti, rec);
    ctx->InitSim(cy::SimInfo(5));

    Inver* a1 = new Inver(ctx);
    cy::DynamicModule::man_agents_["::Inver"] = a1->Clone();
    a1->agent_impl("::Inver");
    a1->val1 = 23;
    Inver* a2 = new Inver(ctx);
    a2->agent_impl("::Inver");
    a2->val1 = 26;

    cy::SimInit::SnapAgent(a1);
    cy::SimInit::SnapAgent(a2);

    ctx->AddPrototype("proto1", a1);
    ctx->AddPrototype("proto2", a2);
    ctx->SchedBuild(NULL, "proto1", 2);
    ctx->SchedBuild(NULL, "proto2", 3);
    a1->Clone()->Build(NULL);
    a2->Clone()->Build(NULL);

    cy::SimInit::Snapshot(ctx);
    rec->Flush();
  }

  virtual void TearDown() {
    delete ctx;
    delete rec;
    delete b;
    remove(dbpath);
  }

  int transid(cy::Context* ctx) {
    return ctx->trans_id_;
  };

  cy::Context* ctx;
  cy::Timer ti;
  cy::Recorder* rec;
  cy::SqliteBack* b;
};

TEST_F(SimInitTest, InitNextIds) {
  cy::SimInit si;
  si.Init(rec, b);
  cy::Context* init_ctx = si.context();

  EXPECT_EQ(transid(ctx), transid(init_ctx));
}
