#include <rclcpp/rclcpp.hpp>  
#include <rclcpp_lifecycle/lifecycle_node.hpp>  // Yaşam döngüsü düğümü için gerekli başlık dosyası.
#include <std_msgs/msg/string.hpp>  // std_msgs paketinden String mesaj türünü kullanmak için.
#include <memory>  // Akıllı işaretçiler için.
#include <thread>  // Uyku ve iş parçacıkları için.

using namespace std::chrono_literals;  // Zaman birimlerini kolayca kullanabilmek için.

class DenemeLifecycleNode : public rclcpp_lifecycle::LifecycleNode
{
public:
    explicit DenemeLifecycleNode(const std::string &node_name, bool intra_process_comms = false)
        : LifecycleNode(node_name, rclcpp::NodeOptions().use_intra_process_comms(intra_process_comms))
    {}  // Düğümü başlatırken adını belirtiyoruz ve intra-process iletişimini opsiyonel olarak ayarlıyoruz.

    // "configure" durumuna geçildiğinde çağrılan fonksiyon.
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_configure(const rclcpp_lifecycle::State &)
    {
        // "chatter" isimli bir topic üzerinden gelen String mesajları almak için bir abonelik oluşturuyoruz.
        sub_ = create_subscription<std_msgs::msg::String>(
            "chatter", 10, std::bind(&DenemeLifecycleNode::msgCallback, this, std::placeholders::_1));

        RCLCPP_INFO(get_logger(), "Lifecycle node on configure() called.");  // Bilgilendirici mesaj yazdırıyoruz.
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;  // Başarıyla tamamlandı.
    }

    // "activate" durumuna geçildiğinde çağrılan fonksiyon.
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_activate(const rclcpp_lifecycle::State &state)
    {
        RCLCPP_INFO(get_logger(), "Lifecycle node on_activate() called.");  // Aktif hale geçildiğini bildiriyoruz.
        std::this_thread::sleep_for(2s);  // 2 saniye bekletiyoruz.
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;  // Başarıyla tamamlandı.
    }

    // "deactivate" durumuna geçildiğinde çağrılan fonksiyon.
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State &state)
    {
        RCLCPP_INFO(get_logger(), "Lifecycle node on deactivate() called.");  // Düğüm devre dışı bırakıldığını bildiriyoruz.
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;  // Başarıyla tamamlandı.
    }

    // "cleanup" durumuna geçildiğinde çağrılan fonksiyon.
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_cleanup(const rclcpp_lifecycle::State &state)
    {
        sub_.reset();  // Abonelik iptal ediliyor.
        RCLCPP_INFO(get_logger(), "Lifecycle node on cleanup() called.");  // Temizleme işlemi yapıldığını bildiriyoruz.
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;  // Başarıyla tamamlandı.
    }

    // "shutdown" durumuna geçildiğinde çağrılan fonksiyon.
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_shutdown(const rclcpp_lifecycle::State &state)
    {
        sub_.reset();  // Abonelik iptal ediliyor.
        RCLCPP_INFO(get_logger(), "Lifecycle node on shutdown() called.");  // Kapanış işlemi yapıldığını bildiriyoruz.
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;  // Başarıyla tamamlandı.
    }

private:
    // Mesaj alındığında çağrılan callback fonksiyon.
    void msgCallback(const std_msgs::msg::String::SharedPtr msg)
    {
        auto state = get_current_state();  // Mevcut düğüm durumunu alıyoruz.
        if (state.label() == "active")  // Düğüm aktif durumda ise mesajı yazdırıyoruz.
        {
            RCLCPP_INFO_STREAM(get_logger(), "Lifecycle node heard: " << msg->data);
        }
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;  // Mesaj aboneliği için işaretçi.
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);  // ROS 2 başlatılıyor.
    rclcpp::executors::SingleThreadedExecutor executor;  // Tek iş parçacıklı yürütücü oluşturuluyor.
    auto node = std::make_shared<DenemeLifecycleNode>("lifecycle_node");  // Yaşam döngüsü düğümü oluşturuluyor.

    executor.add_node(node->get_node_base_interface());  // Düğüm yürütücüye ekleniyor.
    executor.spin();  // Düğüm çalıştırılıyor.

    rclcpp::shutdown();  // ROS 2 kapatılıyor.
    return 0;  // Program sonlandırılıyor.
}
