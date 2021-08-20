#include "pch/pch.h"

#include "minstack/MinStackRenderedItem.h"
#include "minstack/MinStackRendererConfiguration.h"

using testing::Test;

using UKControllerPlugin::MinStack::MinStackRenderedItem;
using UKControllerPlugin::MinStack::MinStackRendererConfiguration;

namespace UKControllerPluginTest::MinStack {

    class MinStackRendererConfigurationTest : public Test
    {
        public:
        MinStackRenderedItem item1{1, "tma.LTMA"};
        MinStackRenderedItem item2{2, "tma.STMA"};
        MinStackRendererConfiguration configuration;
    };

    TEST_F(MinStackRendererConfigurationTest, ItStartsEmpty)
    {
        EXPECT_EQ(0, this->configuration.CountItems());
    }

    TEST_F(MinStackRendererConfigurationTest, ItAddsAnItem)
    {
        this->configuration.AddItem(this->item1);
        this->configuration.AddItem(this->item2);
        EXPECT_EQ(2, this->configuration.CountItems());
    }

    TEST_F(MinStackRendererConfigurationTest, ItDoesntAddDuplicates)
    {
        this->configuration.AddItem(this->item1);
        this->configuration.AddItem(this->item1);
        EXPECT_EQ(1, this->configuration.CountItems());
    }

    TEST_F(MinStackRendererConfigurationTest, ItRemovesItems)
    {
        this->configuration.AddItem(this->item1);
        this->configuration.AddItem(this->item2);
        this->configuration.RemoveItem(this->item1);
        EXPECT_EQ(this->item2, *this->configuration.cbegin());
    }

    TEST_F(MinStackRendererConfigurationTest, ItRemovesItemsByOrder)
    {
        this->configuration.AddItem(this->item1);
        this->configuration.AddItem(this->item2);
        this->configuration.RemoveItem(1);
        EXPECT_EQ(this->item2, *this->configuration.cbegin());
    }

    TEST_F(MinStackRendererConfigurationTest, ItHandlesNonExistantItems)
    {
        EXPECT_NO_THROW(this->configuration.RemoveItem(55));
    }

    TEST_F(MinStackRendererConfigurationTest, ItCanIterateItemsInOrder)
    {
        this->configuration.AddItem(this->item1);
        this->configuration.AddItem(this->item2);
        auto it = this->configuration.cbegin();
        EXPECT_EQ(this->item1, *it++);
        EXPECT_EQ(this->item2, *it);
    }

    TEST_F(MinStackRendererConfigurationTest, ItReturnsInvalidItem)
    {
        this->configuration.AddItem(this->item1);
        this->configuration.AddItem(this->item2);
        EXPECT_EQ(this->configuration.InvalidItem(), this->configuration.GetItem("foo"));
    }

    TEST_F(MinStackRendererConfigurationTest, ItReturnsItemsByKey)
    {
        this->configuration.AddItem(this->item1);
        this->configuration.AddItem(this->item2);
        EXPECT_EQ(this->item1, this->configuration.GetItem("tma.LTMA"));
    }
} // namespace UKControllerPluginTest::MinStack
