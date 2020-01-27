#pragma once
#include <control/Route.hpp>
#include <media.hpp>
#include <codec.hpp>
#include <refi.hpp>
#include <custom.hpp>

namespace fpp {

    class Pipeline;
    using SharedPipeline = std::shared_ptr<Pipeline>;

    enum class SourceType {
        Mandatory,
        Backup,
    };

    class Pipeline : public Thread {

    public:

        Pipeline();
        virtual ~Pipeline() override;

        void                addElement(SharedProcessor processor, SourceType priority = SourceType::Mandatory); //TODO refactoring priority 13.01
                                                                                                                // перенести свойство в процессор
        void                remElement(SharedProcessor processor);
        void                remElement(int64_t uid);

        void                dump() const;

    private:

        virtual void        init()      override;
        virtual Code        run()       override;
        virtual void        onStop()    override;

        Code                determineSequence(const SharedProcessor output_processor);
        Code                createSequence(Route& route);

        SharedProcessor     findProcessor(int64_t uid);
        SharedStream        findStream(uid_t uid);
        SharedStream        findBestInputStream(MediaType type);


        virtual std::string toString() const override;


        Route*              findRoute(const int64_t uid);

        Code                simplifyRoutes(); //TODO доаработать 15.01

    private:

        ProcessorList       _data_sources;
        ProcessorList       _data_sinks;
        ProcessorList       _data_backup_sources; //TODO убрать и объединить с обычными сорсами ? 14.01
        RouteVector         _route_list;

    };

} // namespace fpp
