#pragma once
#include "minstack/MinStackRenderedItem.h"

namespace UKControllerPlugin {
    namespace MinStack {

        /*
            A class that manages the order and
            contents of the minstack display for a given
            rendered instance.
        */
        class MinStackRendererConfiguration
        {
            public:
                void AddItem(const UKControllerPlugin::MinStack::MinStackRenderedItem item);
                size_t CountItems(void) const;
                UKControllerPlugin::MinStack::MinStackRenderedItem GetItem(std::string key) const;
                void RemoveItem(const UKControllerPlugin::MinStack::MinStackRenderedItem item);
                void RemoveItem(unsigned int index);
                void Reset(void);
                void SetShouldRender(bool shouldRender);
                bool ShouldRender(void) const;

                // Public type definitions for a custom iterator over the class.
                typedef std::set<UKControllerPlugin::MinStack::MinStackRenderedItem> MinStackRenderedItems;
                typedef MinStackRenderedItems::const_iterator const_iterator;
                const_iterator cbegin(void) const { return this->items.cbegin(); }
                const_iterator cend(void) const { return this->items.cend(); }

                const UKControllerPlugin::MinStack::MinStackRenderedItem invalidItem = { UINT_MAX, "" };

            private:

                // The items to render
                std::set<UKControllerPlugin::MinStack::MinStackRenderedItem> items;

                // Should the MinStacks be rendered?
                bool shouldRender = true;
        };

    }  // namespace MinStack
}  // namespace UKControllerPlugin
